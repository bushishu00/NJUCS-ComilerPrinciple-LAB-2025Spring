#include "mynode.h"
#include "syntax.tab.h"

#include <stdio.h>
#include <stdlib.h>

extern void yyrestart(FILE* );
extern int yyparse();
extern int yylineno;

Node* root = NULL;/* the root node of Abstract Syntax Tree*/
int errornum = 0;
int errorline = 0;/* make sure only 1 error in one line */

int main(int argc, char** argv){
    if (argc <= 1){/* stdin */
        return 1;
    }
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }

    yylineno = 1;
    yyrestart(f);
    yyparse();
    fclose(f);
    if (errornum == 0){
        print_tree(root, 0);
    }
    

    return 0;
}