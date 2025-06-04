
#include "syntax.tab.h"
#include "semantic.h"
#include "intermediate.h"
#include "objective.h"

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

    FILE *f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }
    FILE *output = fopen(argv[2], "wt+");
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
        translate_Program(root, output);
        generate_object_code(output);
    }
    fclose(output);
    return 0;
}