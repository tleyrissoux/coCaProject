#include "Solving.h"

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

/**
 * @brief Generates a SAT formula satisfiable if and only if all graphs of @p graphs contain an accepting path of length @p pathLength.
 * 
 * @param ctx The solver context.
 * @param graphs An array of graphs.
 * @param numGraphs The number of graphs in @p graphs.
 * @param pathLength The length of the path to check.
 * @return Z3_ast The formula.
 */
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength)
{
  int i,q,j,l,source,target=0;
  Z3_ast chemin_valide[numGraphs];
  Z3_ast source_target[numGraphs][2];
  Z3_ast chemin_simple;
  Z3_ast chemin_simple_graphes[numGraphs];
  Z3_ast chemin_simple_sommets[numGraphs];
  Z3_ast chemin_simple_pos[numGraphs][];
  Z3_ast chemin_simple_and[numGraphs][pathLength];
  Z3_ast chemin_simple_clause1[numGraphs][pathLength];
  Z3_ast chemin_simple_clause2[numGraphs][pathLength];
  Z3_ast chemin_simple_tab_or[numGraphs][pathLength][2];
  Z3_ast chemin_simple_or[numGraphs][pathLength];
  
  
  for(i=0;i<numGraphs;i++)
    {
      for(source=0;source<orderG(graphs[i]) && !isSource(graphs[i],source);source++);
      Z3_ast source_target[i][0] getNodeVariable(ctx,i,0,pathLength,source);
      for(target=0;target<orderG(graphs[i]) && !isTarget(graphs[i],target);target++);
      Z3_ast source_target[i][1] = getNodeVariable(ctx,i,pathLength,pathLength,target);
      chemin_valide[i] = Z3_mk_and(ctx,2,source_target[i]);
      
      chemin_simple_sommets[i] = malloc(orderG[graphs[i]]*sizeof(Z3_ast));
      
      for(q=0;q<orderG(graphs[i]);q++)
	{
	  for(j=0;j<=pathLength;j++)
	    {
	      chemin_simple_tab_or[i][j][1] =Z3_mk_not(ctx,getNodeVariable(ctx,i,j,pathLength,q));
	      chemin_simple_clause1[i][j] = getNodeVariable(ctx,i,j,pathLength,q);
	      for(l=0;l<=pathLength;l++)
	        {
		  if(l!=j)
		    chemin_simple_clause1[i][l] = Z3_mk_not(ctx,getNodeVariable(ctx,i,l,pathLength,q));
	        }
	      chemin_simple_tab_or[i][j][0]=Z3_mk_and(ctx,pathLength,chemin_simple_clause1);
	      chemin_simple_or[i][j] = Z3_mk_or(ctx,2,chemin_simple_tab_or[i][j]);
	    }
	  chemin_simple_sommets[i][q]=Z3_mk_and(ctx,pathLength,chemin_simple_or[i]);
	}
      chemin_simple_graphes[i] = Z3_mk_and(ctx,pathLength,chemin_simple_sommets[i]);
    }
  chemin_simple = Z3_mk_and(ctx,pathLength,chemin_simple_graphes);
  return chemin_simple;
}
  

