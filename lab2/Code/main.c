
#include "syntax.tab.h"
#include "semantic.h"

#include <stdio.h>
#include <stdlib.h>

extern void yyrestart(FILE*);
extern int yyparse();
extern int yylineno;

astNode *root = NULL;/* the root node of Abstract Syntax Tree*/
int errornum = 0;
int errorline = 0;/* make sure only 1 error in one line */

int main(int argc, char** argv){
    if (argc <= 1){/* stdin */
        printf("Error: No input file!\n");
        assert(0);
    }
    for (int i = 1; i < argc; i++){
        FILE *f = fopen(argv[i], "r");
        if (!f){
            perror(argv[i]);
            return 1;
        }
        /* initialize */
        root = NULL;
        yylineno = 1;
        errornum = 0;
        errorline = 0;
        yyrestart(f);
        yyparse();
        fclose(f);
        if (errornum == 0){
            Program(root);
        }
        /* finish a file */
        //printf("Parse done!\n\n");
    }
    return 0;
}