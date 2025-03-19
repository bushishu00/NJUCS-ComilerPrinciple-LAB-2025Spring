#include "astnode.h"
#include "syntax.tab.h"

#include <stdio.h>
#include <stdlib.h>

extern void yyrestart(FILE*);
extern int yyparse();
extern int yylineno;

Node *root = NULL;/* the root node of Abstract Syntax Tree*/
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
        root = NULL;
        yylineno = 1;
        yyrestart(f);
        yyparse();
        fclose(f);
        if (errornum == 0){
            print_tree(root, 0);
        }
        /* finish a file */
        printf("Parse done!\n\n");
    }
    return 0;
}