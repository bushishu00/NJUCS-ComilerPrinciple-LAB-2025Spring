#include "node.h"
#include "syntax.tab.h"

#include <stdio.h>
#include <stdlib.h>

extern void yyrestart(FILE* );
extern int yyparse();
extern int yylineno;



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

    return 0;
}