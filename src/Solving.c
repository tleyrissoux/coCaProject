#include "Solving.h"
#include "Z3Tools.h"
#include <stdlib.h>

#define VAR_SIZE 16

Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node)
{
  Z3_ast res;
  char variable[VAR_SIZE];
  sprintf(variable, "x%d%d%d%d", number, position, k, node);
  res = mk_bool_var(ctx, variable);
  return res;
}

Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength)
{
  Z3_ast formules[3];
  Z3_ast res;
  formules[1] = formule1(ctx, graphs, numGraphs, pathLength);
  formules[2] = Z3_mk_true(ctx); //formule2(ctx, graphs, numGraphs, pathLength);
  formules[3] = formule3(ctx, graphs, numGraphs, pathLength);
  res = Z3_mk_and(ctx, 3, formules);
  return res;
}

Z3_ast formule1(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
  int order;
  Z3_ast tab_i [numGraphs];
  Z3_ast tab_q [2];
  Z3_ast res;
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

Z3_ast formule2(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength);

Z3_ast formule3(Z3_context ctx, Graph *graphs, unsigned int numGraphs, int pathLength)
{
  int order;
  Z3_ast tab_i [numGraphs];
  Z3_ast tab_j [pathLength+1];
  Z3_ast *tab_q;
  Z3_ast *tab_p;
  Z3_ast res;
  for (int i = 0; i < numGraphs; i++) {
    order = orderG(graphs[i]);
    tab_q = (Z3_ast *) malloc(order*sizeof(Z3_ast));
    tab_p = (Z3_ast *) malloc(order*sizeof(Z3_ast));
    for (int j = 0; j <= pathLength; j++) {
      for (int q = 0; q < order; q++) {
	for (int p = 0; p < order; p++) {
	  tab_p[p] = Z3_mk_not(ctx, getNodeVariable(ctx, i, j, pathLength, p));
	}
	tab_p[q] = getNodeVariable(ctx, i, j, pathLength, q);
	tab_q[q] = Z3_mk_and(ctx, order, tab_p);
      }
      tab_j[j] = Z3_mk_or(ctx, order, tab_q);
    }
    tab_i[i] = Z3_mk_and(ctx, pathLength+1, tab_j);
    free(tab_q);
    free(tab_p);
  }
  res = Z3_mk_and(ctx, numGraphs, tab_i);
  return res;
}

/*
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength)
{
  int i,q,j,l,source,target=0;
  Z3_ast chemin_valide[numGraphs];
  Z3_ast source_target[numGraphs][2];
  Z3_ast chemin_simple;
  Z3_ast chemin_simple_graphes[numGraphs];
  Z3_ast chemin_simple_sommets[numGraphs][];
  Z3_ast chemin_simple_and[numGraphs][pathLength];
  Z3_ast chemin_simple_clause1[numGraphs][pathLength];
  Z3_ast chemin_simple_clause2[numGraphs][pathLength];
  Z3_ast chemin_simple_tab_or[numGraphs][pathLength][2];
  Z3_ast chemin_simple_or[numGraphs][pathLength];
  Z3_ast chemin_longueur_clause1[numGraphs][pathLength];
  
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
  for(i=0;i<numGraphs;i++)
    {
      for(q=0;q<orderG(graphs[i]);q++)
	{
	  for(j=0;j<=pathLength;j++)
	    {
	      chemin_longueur_clause1[i][j]
	    }
	}

    }
  return chemin_simple;
}
*/
