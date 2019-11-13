#include "Solving.h"
#include <stdio.h>
Z3_ast validPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast simplePathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast pathLengthFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
int maximumOrder(Graph *graphs,unsigned int numGraphs);
int minimumOrder(Graph *graphs,unsigned int numGraphs);

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node)
{
  /**
 * @brief Generates a formula consisting of a variable representing the fact that @p node of graph number @p number is at position @p position of an accepting path.
 * 
 * @param ctx The solver context.
 * @param number The number of the graph.
 * @param position The position in the path.
 * @param k The mysterious k from the subject of this assignment.
 * @param node The node identifier.
 * @return Z3_ast The formula.
 */

  char variable[20];
  sprintf(variable,"x_%d_%d_%d_%d", number, position, k, node);
  return mk_bool_var(ctx,variable);
}

int maximumOrder(Graph *graphs,unsigned int numGraphs){
  int i,max = 0;
  for(i=0;i<numGraphs;i++){
    if(orderG(graphs[i])>max)
       max = orderG(graphs[i]);
  }
  return max;
}

int minimumOrder(Graph *graphs,unsigned int numGraphs){
  int i,min;
  min=orderG(graphs[0]);
  for(i=0;i<numGraphs;i++){
    if(orderG(graphs[i])<min)
       min = orderG(graphs[i]);
  }
  return min;
}

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast graphsToPathFormula(Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength)
{
  
  Z3_ast formules[3];
  Z3_ast res;
  if(pathLength>minimumOrder(graphs,numGraphs))
     return Z3_mk_false(ctx);
  formules[0] = validPathFormula(ctx, graphs, numGraphs, pathLength);
  printf(Z3_ast_to_string(ctx,formules[0]));
  printf("\n");
  formules[1] = simplePathFormula(ctx, graphs, numGraphs, pathLength);
  printf(Z3_ast_to_string(ctx,formules[1]));
  printf("\n");
  formules[2] = pathLengthFormula(ctx, graphs, numGraphs, pathLength);
  printf(Z3_ast_to_string(ctx,formules[2]));
  printf("\n");
  res = Z3_mk_and(ctx, 3, formules);
  return res;
}
  

Z3_ast validPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
  int order;
  Z3_ast tab_i [numGraphs];
  Z3_ast tab_q [2];
  Z3_ast res;
  int node;
  for (int i = 0; i < numGraphs; i++) {
    order = orderG(graphs[i]);
    tab_q[0] = Z3_mk_false(ctx);
    tab_q[1] = Z3_mk_false(ctx);
    for (int q = 0; q < order; q++) {
      if (isSource(graphs[i], q))
	tab_q[0] = getNodeVariable(ctx, i, 0, pathLength, q);
      if (isTarget(graphs[i], q))
	tab_q[1] = getNodeVariable(ctx, i, pathLength, pathLength, q);
    }
    tab_i[i] = Z3_mk_and(ctx, 2, tab_q);
  }
  res = Z3_mk_and(ctx, numGraphs, tab_i);
  return res;
}

Z3_ast simplePathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength){
  int max = maximumOrder(graphs,numGraphs);
  int i,q,j,l;
  Z3_ast chemin_simple;
  Z3_ast chemin_simple_graphes[numGraphs];
  Z3_ast chemin_simple_sommets[numGraphs][max];
  Z3_ast chemin_simple_and[numGraphs][max][pathLength+1];
  Z3_ast chemin_simple_clause1[numGraphs][max][pathLength+1][pathLength+1];
  Z3_ast chemin_simple_tab_or[numGraphs][max][pathLength+1][2];
  Z3_ast chemin_simple_or[numGraphs][max][pathLength+1];
  for(i=0;i<numGraphs;i++)
    {
      for(q=0;q<orderG(graphs[i]);q++)
	{
	  for(j=0;j<=pathLength;j++)
	    {
	      chemin_simple_tab_or[i][q][j][1] =Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,q));
	      chemin_simple_clause1[i][q][j][j] =getNodeVariable(ctx,i,j,pathLength,q);
	      for(l=0;l<=pathLength;l++)
	        {
		  if(l!=j)
		    chemin_simple_clause1[i][q][j][l] = Z3_mk_not(ctx,getNodeVariable(ctx,i,l,pathLength,q));
	        }
	      chemin_simple_tab_or[i][q][j][0]=Z3_mk_and(ctx,pathLength,chemin_simple_clause1[i][q][j]);
	      chemin_simple_or[i][q][j] = Z3_mk_or(ctx,2,chemin_simple_tab_or[i][q][j]);
	    }	  chemin_simple_sommets[i][q]=Z3_mk_and(ctx,pathLength,chemin_simple_or[i][q]);
	}
      chemin_simple_graphes[i] = Z3_mk_and(ctx,pathLength,chemin_simple_sommets[i]);
    }
  chemin_simple = Z3_mk_and(ctx,numGraphs,chemin_simple_graphes);
  return chemin_simple;
}


Z3_ast pathLengthFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
  int order = maximumOrder(graphs,numGraphs);
  Z3_ast tab_i [numGraphs];
  Z3_ast tab_j [pathLength+1];
  Z3_ast tab_q[order];
  Z3_ast tab_p[order];
  Z3_ast res;
  for (int i = 0; i < numGraphs; i++) {
    for (int j = 0; j <= pathLength; j++) {
      for (int q = 0; q < order; q++) {
	for (int p = 0; p < order; p++) {
	  if(p!=q)
	    tab_p[p] = Z3_mk_not(ctx, getNodeVariable(ctx, i, j, pathLength, p));
	}
	tab_p[q] = getNodeVariable(ctx, i, j, pathLength, q);
	tab_q[q] = Z3_mk_and(ctx, order, tab_p);
      }
      tab_j[j] = Z3_mk_or(ctx, order, tab_q);
    }
    tab_i[i] = Z3_mk_and(ctx, pathLength+1, tab_j);
  }
  res = Z3_mk_and(ctx, numGraphs, tab_i);
  return res;
}


