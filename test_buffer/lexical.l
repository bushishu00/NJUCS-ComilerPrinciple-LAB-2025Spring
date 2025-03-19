%{
    #include "mynode.h"
    #include "syntax.tab.h"

    #include <stdio.h>
    #include <stdlib.h>

    extern int errornum;
    extern int errorline;
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
FLOAT       {digit}+\.{digit}+
FLOAT_exp   (({digit}*\.{digit}+)|({digit}+\.{digit}*))([eE][+-]?{digit}+)
FLOAT_ERROR (\.{digit}|{digit}\.|{digit}\.{digit})([eE][+-]?)
INT_oct     0([0-7]{1,11})
OCT_ERROR   0[0-7]*[8-9]+[0-9]*
INT_hex     0[xX]([0-9a-fA-F]{1,8})
HEX_ERROR   0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
INT         0|([1-9]([0-9]{0,9}))
DEC_ERROR   [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]*
ID          {_letter}({_ld}{0,31})
ID_ERROR    {digit}({_letter}|{digit})*
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
%%
{ws}            { /* do nothing*/ }      
{INT}           { yylval.node = create_node(yylineno, "INT", yytext, LEX_NODE);
                  return INT;}
{DEC_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal dec number '%s'\n", yylineno, yytext);
                        yylval.node = create_node(yylineno, "INT", yytext, LEX_NODE);
                        return INT;
                    }
                }
{INT_oct}       { yylval.node = create_node(yylineno, "INT_oct", yytext, LEX_NODE);
                  return INT;/* recognize as INT */}
{OCT_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal oct number '%s'\n", yylineno, yytext);
                        yylval.node = create_node(yylineno, "INT", yytext, LEX_NODE);
                        return INT;
                    }
                }
{INT_hex}       { yylval.node = create_node(yylineno, "INT_hex", yytext, LEX_NODE);
                  return INT;/* recognize as INT */}
{HEX_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal hex number '%s'\n", yylineno, yytext);
                        yylval.node = create_node(yylineno, "INT", yytext, LEX_NODE);
                        return INT;
                    }
                }
{FLOAT}         { yylval.node = create_node(yylineno, "FLOAT", yytext, LEX_NODE);
                  return FLOAT;}
{FLOAT_exp}     { yylval.node = create_node(yylineno, "FLOAT_exp", yytext, LEX_NODE);
                  return FLOAT;}/* recognize as FLOAT */  
{FLOAT_ERROR}   {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal float number '%s'\n", yylineno, yytext);
                        yylval.node = create_node(yylineno, "FLOAT", yytext, LEX_NODE);
                        return FLOAT;
                    }
                } 
{RELOP}         { yylval.node = create_node(yylineno, "RELOP", yytext, LEX_NODE); 
                  return RELOP;}
{SEMI}          { yylval.node = create_node(yylineno, "SEMI", yytext, LEX_NODE); 
                  return SEMI;}
{COMMA}         { yylval.node = create_node(yylineno, "COMMA", yytext, LEX_NODE); 
                  return COMMA;}
{ASSIGNOP}      { yylval.node = create_node(yylineno, "ASSIGNOP", yytext, LEX_NODE); 
                  return ASSIGNOP;}
{PLUS}          { yylval.node = create_node(yylineno, "PLUS", yytext, LEX_NODE); 
                  return PLUS;}
{MINUS}         { yylval.node = create_node(yylineno, "MINUS", yytext, LEX_NODE); 
                  return MINUS;}
{STAR}          { yylval.node = create_node(yylineno, "STAR", yytext, LEX_NODE); 
                  return STAR;}
{DIV}           { yylval.node = create_node(yylineno, "DIV", yytext, LEX_NODE); 
                  return DIV;}
{AND}           { yylval.node = create_node(yylineno, "AND", yytext, LEX_NODE); 
                  return AND;}
{OR}            { yylval.node = create_node(yylineno, "OR", yytext, LEX_NODE); 
                  return OR;}
{DOT}           { yylval.node = create_node(yylineno, "DOT", yytext, LEX_NODE); 
                  return DOT;}
{NOT}           { yylval.node = create_node(yylineno, "NOT", yytext, LEX_NODE); 
                  return NOT;}
{TYPE}          { yylval.node = create_node(yylineno, "TYPE", yytext, LEX_NODE); 
                  return TYPE;}
{LP}            { yylval.node = create_node(yylineno, "LP", yytext, LEX_NODE); 
                  return LP;}
{RP}            { yylval.node = create_node(yylineno, "RP", yytext, LEX_NODE); 
                  return RP;}
{LB}            { yylval.node = create_node(yylineno, "LB", yytext, LEX_NODE); 
                  return LB;}
{RB}            { yylval.node = create_node(yylineno, "RB", yytext, LEX_NODE); 
                  return RB;}
{LC}            { yylval.node = create_node(yylineno, "LC", yytext, LEX_NODE); 
                  return LC;}
{RC}            { yylval.node = create_node(yylineno, "RC", yytext, LEX_NODE); 
                  return RC;}
{STRUCT}        { yylval.node = create_node(yylineno, "STRUCT", yytext, LEX_NODE); 
                  return STRUCT;}
{RETURN}        { yylval.node = create_node(yylineno, "RETURN", yytext, LEX_NODE); 
                  return RETURN;}
{IF}            { yylval.node = create_node(yylineno, "IF", yytext, LEX_NODE); 
                  return IF;}
{ELSE}          { yylval.node = create_node(yylineno, "ELSE", yytext, LEX_NODE); 
                  return ELSE;}
{WHILE}         { yylval.node = create_node(yylineno, "WHILE", yytext, LEX_NODE);
                  return WHILE;}
{ID}            { yylval.node = create_node(yylineno, "ID", yytext, LEX_NODE);
                  return ID;}   
{ID_ERROR}      {   if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal id '%s'\n", yylineno, yytext);
                        yylval.node = create_node(yylineno, "ID", yytext, LEX_NODE);
                        return ID;
                    }
                }  
"//"            {
                char c = input();
                int skipnum = 1;
                while (c != '\n'){
                    c = input();
                }
                //printf("\'//\' comment detected, %d characters is skipped\n", skipnum);
                }
"/*"            {
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
                //printf("\'/**/\' comment detected, %d lines is skipped\n", skipnum);
                }
\n              { yycolumn = 1; }
.               { 
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Mysterious characters '%s'\n", yylineno, yytext);
                    }
                }
%%
/* main function is implemented in main.c */