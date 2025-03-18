#include "node.h"

#include <stdio.h>
#include <stdlib.h>

extern void yyrestart(FILE *);
extern int yylineno;



int main(int argc, char** argv){

    if (argc < 2){/* stdin */
        return 1;
    }
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);
        return 1;
    }

    yylineno = 1;
    yyrestart(f);
    yylex();
    fclose(f);

    return 0;
}