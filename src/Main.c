#include <stdio.h>
#include "Graph.h"
#include <Parsing.h>
#include "Solving.h"
#include <string.h>

int main(int argc, char* argv[])
{
  Z3_context ctx = makeContext();
  Z3_ast formule;
  if (strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-g") != 0) {
    Graph graphs[argc-1];
    for (int i = 1; i < argc; i++) {
      graphs[i-1] = getGraphFromFile(argv[i]);
    }
    formule = graphsToFullFormula(ctx, graphs, argc-1);
  }
  else {
    Graph graphs[argc-2];
    for(int i = 2; i < argc; i++) {
      graphs[i-2] = getGraphFromFile(argv[i]);
    }
    formule = graphsToFullFormula(ctx, graphs, argc-2);
    if (strcmp(argv[1], "-f") == 0) { //print formula
      printf("\nSAT Formula : \n");
      printf(Z3_ast_to_string(ctx, formule));
    }
    else {
      if (strcmp(argv[1], "-g") == 0) { //print graphes
	for (int i = 2; i < argc; i++) {
	  printf("\nGraphe %d : \n", i-1);
	  printGraph(graphs[i-2]);
	}
      }
      else {
	printf("Unvalid parameters ! \n");
	return 1;
      }
    }
    printf("\n\n");
  }
  Z3_lbool isSat = isFormulaSat(ctx, formule);
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
