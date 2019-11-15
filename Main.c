#include <stdio.h>
#include "Graph.h"
#include <Parsing.h>
#include "Solving.h"
#include <string.h>

void print_formula(Z3_context ctx, Z3_ast formula) {
  printf("\nSAT formula : \n");
  printf(Z3_ast_to_string(ctx, formula));
}

void print_graphes(Graph *graphs, int shift, int argc) {
  for (int i = shift; i < argc; i++) {
    printf("\nGraphe %d : \n", i-shift+1);
    printGraph(graphs[i-shift]);
  }
}

int main(int argc, char* argv[])
{
  Z3_context ctx = makeContext();
  Z3_ast formula;
  int f1 = strcmp(argv[1], "-f");
  int f2 = strcmp(argv[2], "-f");
  int g1 = strcmp(argv[1], "-g");
  int g2 = strcmp(argv[2], "-g");
  if (f1 != 0 && g1 != 0) {
    Graph graphs[argc-1];
    for (int i = 1; i < argc; i++) {
      graphs[i-1] = getGraphFromFile(argv[i]);
    }
    formula = graphsToFullFormula(ctx, graphs, argc-1);
  }
  else {
    if ((f1 == 0 && g2 != 0) || (g1 == 0 && f2 != 0)) {
      Graph graphs[argc-2];
      for (int i = 2; i < argc; i++) {
	graphs[i-2] = getGraphFromFile(argv[i]);
      }
      formula = graphsToFullFormula(ctx, graphs, argc-2);
      if (f1 == 0 && g2 != 0) { //print formula
	print_formula(ctx, formula);
      }
      else { //print graphes
	print_graphes(graphs, 2, argc);
      }
    }
    else {
      if ((g1 == 0 && f2 == 0) || (f1 == 0 && g2 == 0)) {
	Graph graphs[argc-3];
	for (int i = 3; i < argc; i++) {
	  graphs[i-3] = getGraphFromFile(argv[i]);
	}
	formula = graphsToFullFormula(ctx, graphs, argc-3);
	print_formula(ctx, formula);
	printf("\n");
	print_graphes(graphs, 3, argc);
      }
      else {
	printf("Unvalid parameters ! \n");
	return 1;
      }
    }
    printf("\n\n");
  }
  Z3_lbool isSat = isFormulaSat(ctx, formula);
  switch (isSat) {
  case Z3_L_FALSE:
    printf("The answer of the problem is no...\n");
    break;
  case Z3_L_TRUE:
    printf("The answer of the problem is yes ! \n");
    break;
  default:
    printf("The program has encountered issues to find the answer of the problem... \n");
    return 1;
  }
  return 0;
}
