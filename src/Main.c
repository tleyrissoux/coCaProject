#include <stdio.h>
#include "Graph.h"
#include <Parsing.h>
#include "Solving.h"
#include <stdio.h>

int main(int argc, char* argv[]){
  /*Z3_context ctx = makeContext();
  Graph graphs[2];
  graphs[0] = getGraphFromFile(argv[1]);
  graphs[1] = getGraphFromFile(argv[2]);
  printGraph(graphs[0]);
  printGraph(graphs[1]);
  printf("graphMain3\n");
  Z3_ast formule = simplePathFormula(ctx,graphs,2,3);
  printf("graphMain4\n");
  Z3_lbool isSat = isFormulaSat(ctx,formule);
  printf("graphMain5\n");
  switch (isSat)
   {
   case Z3_L_FALSE:
     printf("not satisfiable.\n");
     break;
     
   case Z3_L_UNDEF:
     printf("We don't know if it is satisfiable.\n");
     break;
     
   case Z3_L_TRUE:
     printf("t'as dead ça bro");
     break;
     }*/
  Z3_context ctx = makeContext();
    printf("Creating the context. Must be destroyed at the end of the program.\n");

    Z3_ast x = mk_bool_var(ctx,"x");
    printf("Variable %s created.\n",Z3_ast_to_string(ctx,x));
    printf( Z3_ast_to_string(ctx,x));
    Z3_ast y = mk_bool_var(ctx,"y");
    printf(Z3_ast_to_string(ctx,y));
    printf("Variable %s created.\n",Z3_ast_to_string(ctx,y));

    Z3_ast negX = Z3_mk_not(ctx,x);
    Z3_ast_to_string(ctx,negX);
    printf("Formula %s created.\n",Z3_ast_to_string(ctx,negX));

    Z3_ast absurdTab[3] = {x,y,negX};
    Z3_ast absurd = Z3_mk_and(ctx,3,absurdTab);
    printf("Formula %s created.\n",Z3_ast_to_string(ctx,absurd));

    Z3_ast anOtherTab[3] = {negX,y,absurd};
    Z3_ast easy = Z3_mk_or(ctx,3,anOtherTab);
    printf("We have now: %s\n\n",Z3_ast_to_string(ctx,easy));

    Z3_lbool isSat = isFormulaSat(ctx,absurd);

        switch (isSat)
        {
        case Z3_L_FALSE:
            printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,absurd));
            break;

        case Z3_L_UNDEF:
                printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,absurd));
            break;

        case Z3_L_TRUE:
                printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,absurd));
                Z3_model model = getModelFromSatFormula(ctx,absurd);
                printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,absurd));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
                break;
        }

    isSat = isFormulaSat(ctx,easy);
    printf("\n");

        switch (isSat)
        {
        case Z3_L_FALSE:
            printf("%s is not satisfiable.\n",Z3_ast_to_string(ctx,easy));
            break;

        case Z3_L_UNDEF:
                printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,easy));
            break;

        case Z3_L_TRUE:
                printf("%s is satisfiable.\n",Z3_ast_to_string(ctx,easy));
                Z3_model model = getModelFromSatFormula(ctx,easy);
                printf("Model obtained for %s:\n",Z3_ast_to_string(ctx,easy));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,x),valueOfVarInModel(ctx,model,x));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,y),valueOfVarInModel(ctx,model,y));
                printf("    The value of %s is %d\n",Z3_ast_to_string(ctx,negX),valueOfVarInModel(ctx,model,negX));
                break;
        }

    Z3_del_context(ctx);
    printf("Context deleted, memory is now clean.\n");
    
    return 0;
}
