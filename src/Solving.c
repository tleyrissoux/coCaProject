#include "Solving.h"
#include <stdio.h>
#include <stdlib.h>
#include "Z3Tools.h"
Z3_ast validPathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast simplePathFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast pathLengthFormula(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);
Z3_ast edgeExistsFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs,int pathLength);
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
  
  Z3_ast formules[4];
  Z3_ast res;
  if(pathLength>=minimumOrder(graphs,numGraphs) || pathLength == 0)
     return Z3_mk_false(ctx);
  formules[0] = validPathFormula(ctx, graphs, numGraphs, pathLength);
  formules[1] = simplePathFormula(ctx, graphs, numGraphs, pathLength);
  formules[2] = pathLengthFormula(ctx, graphs, numGraphs, pathLength);
  formules[3] = edgeExistsFormula(ctx, graphs, numGraphs, pathLength);
  res = Z3_mk_and(ctx, 4, formules);
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
  Z3_ast chemin_simple_sommets[max];
  Z3_ast chemin_simple_l[pathLength+1][pathLength+1];
  Z3_ast chemin_simple_j[pathLength+1][2];
  Z3_ast chemin_simple_or[pathLength+1];
  for(i=0;i<numGraphs;i++)
    {
      for(q=0;q<orderG(graphs[i]);q++)
	{
	  for(j=0;j<=pathLength;j++)
	    {
	      chemin_simple_j[j][1] = Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,q));
	      for(l=0;l<=pathLength;l++)
	        {
		    chemin_simple_l[j][l] = Z3_mk_not(ctx,getNodeVariable(ctx,i,l,pathLength,q));
	        }
	      chemin_simple_l[j][j] = getNodeVariable(ctx,i,j,pathLength,q);
	      chemin_simple_j[j][0]=Z3_mk_and(ctx,pathLength+1,chemin_simple_l[j]);
	      chemin_simple_or[j] = Z3_mk_or(ctx,2,chemin_simple_j[j]);
	    }
	  chemin_simple_sommets[q]=Z3_mk_and(ctx,pathLength+1,chemin_simple_or);
	}
      chemin_simple_graphes[i] = Z3_mk_and(ctx,orderG(graphs[i]),chemin_simple_sommets);
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
    order = orderG(graphs[i]);
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

Z3_ast edgeExistsFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs,int pathLength){
  int order = maximumOrder(graphs, numGraphs);
  int currentPos = 0;				      
  Z3_ast tab_i [numGraphs];
  Z3_ast tab_j [pathLength];
  Z3_ast tab_q[order];
  Z3_ast tab_p[order][2];
  Z3_ast tab_and[order*order];
  Z3_ast res;
  for (int i = 0; i < numGraphs; i++) {
    order = orderG(graphs[i]);
    for (int j = 0; j < pathLength; j++) {
      currentPos = 0;
      for (int q = 0; q < order; q++) {
	tab_q[q] = getNodeVariable(ctx,i,j,pathLength,q);
	for (int p = 0; p < order; p++) {
	    if(isEdge(graphs[i],q,p)){
	      tab_p[p][0] = tab_q[q];
	      tab_p[p][1] = getNodeVariable(ctx,i,j+1,pathLength,p);
	      tab_and[currentPos] = Z3_mk_and(ctx,2,tab_p[p]);
	      currentPos++;
	    }
	}
      }
      tab_j[j] = Z3_mk_or(ctx,currentPos,tab_and);
    }
    tab_i[i] = Z3_mk_and(ctx,pathLength,tab_j);
  }
  res = Z3_mk_and(ctx,numGraphs,tab_i);
  return res;
}



Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs){
 
  Z3_ast formule_finale;
  Z3_ast temp;
  int boole = 0;
  int minOrder = minimumOrder(graphs,numGraphs);
  Z3_ast tab_formules[minOrder];
  Z3_model z;
  Z3_lbool b;
  for(int i=0;i<minOrder;i++){
    tab_formules[i] = graphsToPathFormula(ctx,graphs,numGraphs,i);
    if(boole != 1){
      b = isFormulaSat(ctx,tab_formules[i]);
      if(b==Z3_L_TRUE){
	printf("\n Chemin trouvé de taille %d :\n",i); 
	z = getModelFromSatFormula(ctx,tab_formules[i]);
	printPathsFromModel(ctx, z, graphs, numGraphs, i);
	boole = 1;
      }
    }
  }
  formule_finale = Z3_mk_or(ctx,minOrder,tab_formules);
  return formule_finale;
}

void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength)
{
  int order;
  bool value = false;
  for (int i = 0; i < numGraph; i++) {
    order = orderG(graphs[i]);
    printf("Chemin du graphe %d: \n", i+1);
    for (int j = 0; j <= pathLength; j++) {
      for (int q = 0; q < order; q++) {
	value = valueOfVarInModel(ctx, model, getNodeVariable(ctx, i, j, pathLength, q));
	if (value == true) {
	  if (j != 0)
	    printf("->");
	  printf(getNodeName(graphs[i], q));
	}
      }
    }
    printf("\n");
  }
}
