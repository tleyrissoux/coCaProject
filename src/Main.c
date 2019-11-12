#include <stdio.h>
#include "Graph.h"
#include <Parsing.h>
#include "Solving.h"
#include <stdio.h>

int main(int argc, char* argv[]){
  Z3_context ctx = makeContext();
  Graph graphs[2];
  graphs[0] = getGraphFromFile(argv[1]);
  graphs[1] = getGraphFromFile(argv[2]);
  printGraph(graphs[0]);
  printGraph(graphs[1]);
  Z3_ast formule = simplePathFormula(ctx,graphs,2,3);
  Z3_lbool isSat = isFormulaSat(ctx,formule);
  switch (isSat)
   {
   case Z3_L_FALSE:
     printf("not satisfiable.\n");
     break;
     
   case Z3_L_UNDEF:
     printf("We don't know if it is satisfiable.\n");
     break;
     
   case Z3_L_TRUE:
     printf("Satisfiable\n");
     break;
     }
}
