%{
    #include "mynode.h"
    #include "syntax.tab.h"

    #include <stdio.h>
    #include <stdlib.h>

    /* record the token position, refer the code in guidance*/
    int yycolumn = 1; 
    #define YY_USER_ACTION \ 
        yylloc.first_line = yylloc.last_line = yylineno; \ 
        yylloc.first_column = yycolumn; \ 
        yylloc.last_column = yycolumn + yyleng - 1; \ 
        yycolumn += yyleng; 
%}
%option yylineno
ws          [ \t\r]+
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
FLOAT       {digit}+\.{digit}+
FLOAT_exp   (({digit}*\.{digit}+)|({digit}+\.{digit}*))([eE][+-]?{digit}+)
INT         0|([1-9]([0-9]{0,9}))
INT_oct     0([0-7]{1,11})
INT_hex     0[xX]([0-9a-fA-F]{1,8})
ID          {_letter}({_ld}{0,31})
%%
{ws}        { /* do nothing*/ }
{RELOP}     { yylval.node = create_node(yylineno, "RELOP", yytext); 
              return RELOP;}
{SEMI}      { yylval.node = create_node(yylineno, "SEMI", yytext); 
              return SEMI;}
{COMMA}     { yylval.node = create_node(yylineno, "COMMA", yytext); 
              return COMMA;}
{ASSIGNOP}  { yylval.node = create_node(yylineno, "ASSIGNOP", yytext); 
              return ASSIGNOP;}
{PLUS}      { yylval.node = create_node(yylineno, "PLUS", yytext); 
              return PLUS;}
{MINUS}     { yylval.node = create_node(yylineno, "MINUS", yytext); 
              return MINUS;}
{STAR}      { yylval.node = create_node(yylineno, "STAR", yytext); 
              return STAR;}
{DIV}       { yylval.node = create_node(yylineno, "DIV", yytext); 
              return DIV;}
{AND}       { yylval.node = create_node(yylineno, "AND", yytext); 
              return AND;}
{OR}        { yylval.node = create_node(yylineno, "OR", yytext); 
              return OR;}
{DOT}       { yylval.node = create_node(yylineno, "DOT", yytext); 
              return DOT;}
{NOT}       { yylval.node = create_node(yylineno, "NOT", yytext); 
              return NOT;}
{TYPE}      { yylval.node = create_node(yylineno, "TYPE", yytext); 
              return TYPE;}
{LP}        { yylval.node = create_node(yylineno, "LP", yytext); 
              return LP;}
{RP}        { yylval.node = create_node(yylineno, "RP", yytext); 
              return RP;}
{LB}        { yylval.node = create_node(yylineno, "LB", yytext); 
              return LB;}
{RB}        { yylval.node = create_node(yylineno, "RB", yytext); 
              return RB;}
{LC}        { yylval.node = create_node(yylineno, "LC", yytext); 
              return LC;}
{RC}        { yylval.node = create_node(yylineno, "RC", yytext); 
              return RC;}
{STRUCT}    { yylval.node = create_node(yylineno, "STRUCT", yytext); 
              return STRUCT;}
{RETURN}    { yylval.node = create_node(yylineno, "RETURN", yytext); 
              return RETURN;}
{IF}        { yylval.node = create_node(yylineno, "IF", yytext); 
              return IF;}
{ELSE}      { yylval.node = create_node(yylineno, "ELSE", yytext); 
              return ELSE;}
{WHILE}     { yylval.node = create_node(yylineno, "WHILE", yytext);
              return WHILE;}
{ID}        { yylval.node = create_node(yylineno, "ID", yytext);
              return ID;}
{FLOAT}     { yylval.node = create_node(yylineno, "FLOAT", yytext);
              return FLOAT;}
{FLOAT_exp} { yylval.node = create_node(yylineno, "FLOAT_exp", yytext);
              return FLOAT;}/* recognize as FLOAT */              
{INT}       { yylval.node = create_node(yylineno, "INT", yytext);
              return INT;}
{INT_oct}   { yylval.node = create_node(yylineno, "INT_oct", yytext);
              return INT;/* recognize as INT */}
{INT_hex}   { yylval.node = create_node(yylineno, "INT_hex", yytext);
              return INT;/* recognize as INT */}
"//"        {
            char c = input();
            int skipnum = 1;
            while (c != '\n'){
                c = input();
            }
            printf("\'//\' comment detected, %d characters is skipped", skipnum);
            }
"/*"        {
            char c, last_c = 0;
            int skipnum = 1;
            while ((c = input()) != EOF){
                if (c == '/' && last_c == '*'){
                    break;
                }
                if (c == '\n'){
                    skipnum++;
                }
                last_c = c; 
            }
            printf("\'/**/\' comment detected, %d lines is skipped", skipnum);
            }
\n          { yycolumn = 1; }
.           { printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
/* main function is implemented in main.c */