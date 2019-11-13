#include <stdio.h>
#include "Graph.h"
#include <Parsing.h>
#include "Solving.h"
#include <stdio.h>

int main(int argc, char* argv[]){
  Z3_context ctx = makeContext();
  Graph graphs[argc-1];
  for(int i=1;i<argc;i++){
    printf("%d",argc);
    printf("'%s'\n",argv[i]);
    graphs[i-1] = getGraphFromFile(argv[i]);
  }
  Z3_ast formule = graphsToPathFormula(ctx,graphs,argc-1,3);
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
