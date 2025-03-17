%{
#include <stdio.h>
%}
%option yylineno
ws          [ \t]
_letter     [_a-zA-Z]
digit       [0-9]
_ld          [_0-9a-zA-Z]
RELOP       >|<|>=|<=|==|!=
SEMI        ";"
COMMA       ","
ASSIGNOP    "="
PLUS        "+"
MINUS       "-"
STAR        "*"
DIV         "/"
AND         "&&"
OR          "||"
DOT         "."
NOT         "!"
TYPE        int|float
LP          "("
RP          ")"
LB          "["
RB          "]"
LC          "{"
RC          "}"
STRUCT      struct
RETURN      return
IF          if
ELSE        else
WHILE       while
normfloat   {digit}+\.{digit}+
expfloat    (({digit}*\.{digit}+)|({digit}+\.{digit}*))([eE][+-]?{digit}+)
dec         0|([1-9]([0-9]{0,9}))
oct         0([0-7]{1,11})
hex         0[xX]([0-9a-fA-F]{1,8})
ID          {_letter}({_ld}{0,31})
FLOAT       {normfloat}|{expfloat}
INT         {dec}|{oct}|{hex}
%%
{ws}        { /* do nothing*/ }
"\n"        { yycol = 1; }
{IF}        {  }


%%

int main(argc, char** argv){
    if (argc < 2) {/*only read stdin*/
    /*add my code here*/
    }
    for (int i = 1; i < arc; i++){
        FILE *f = fopen(argv[i]. "r");
        yylineno = 1;/*reset the yylineno because it is global*/
        if (!f){/*NULL*/
            perror(argc[i]);
            return 1;
        }
        yyrestart(f);
        /*Add your code here*/
        yylex();
        
        fclose(f);

        
    }
    /*add my code here*/
    return 0;
}