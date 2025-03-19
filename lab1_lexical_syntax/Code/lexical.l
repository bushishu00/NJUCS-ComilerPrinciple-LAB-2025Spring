%{
    #include "astnode.h"
    /* Attention: YYSTYPE has to be defined before include "syntax.tab.h" */
    #define YYSTYPE Node*
    #include "syntax.tab.h"

    #include <stdio.h>
    #include <stdlib.h>
    

    extern int errornum;
    extern int errorline;
    /* record the token position, refer the code in guidance*/
    /* !!QUESTION!!: what's the meaning of column, they don't even consider the \t and space */
    int yycolumn = 1; 
    #define YY_USER_ACTION \ 
        yylloc.first_line = yylloc.last_line = yylineno; \ 
        yylloc.first_column = yycolumn; \ 
        yylloc.last_column = yycolumn + yyleng - 1; \ 
        yycolumn += yyleng; 

    Node *create_lexnode(int lineno, char *name, char *value){
        Node *ret = create_node(LEX_NODE, lineno, name, value, 0);
        return ret;
    }
%}
%option yylineno
ws              [ \t\r]+
_letter         [_a-zA-Z]
digit           [0-9]
_ld             [_0-9a-zA-Z]
FLOAT           {digit}+\.{digit}+
FLOAT_exp       (({digit}*\.{digit}+)|({digit}+\.{digit}*))([eE][+-]?{digit}+)
FLOAT_ERROR     (({digit}*\.{digit}+)|({digit}+\.{digit}*))([eE]?[+-]?)
INT_oct         0([0-7]{1,11})
OCT_ERROR       0[0-7]*[8-9]+[0-9]*
INT_hex         0[xX]([0-9a-fA-F]{1,8})
HEX_ERROR       0[xX][0-9a-fA-F]*[g-zG-Z]+[0-9a-zA-Z]*
INT             0|([1-9]([0-9]{0,9}))
DEC_ERROR       [0-9]+[a-wA-Wy-zY-Z]+[0-9a-dA-Df-zF-Z]*|[0-9]+[0-9]+[a-dA-Df-zF-Z]+[0-9]*[0-9a-dA-Df-zF-Z]*
ID              {_letter}({_ld}{0,31})
ID_ERROR        {digit}({_letter}|{digit})*
RELOP           >|<|>=|<=|==|!=
SEMI            ";"
COMMA           ","
ASSIGNOP        "="
PLUS            "+"
MINUS           "-"
STAR            "*"
DIV             "/"
AND             "&&"
OR              "||"
DOT             "."
NOT             "!"
TYPE            int|float
LP              "("
RP              ")"
LB              "["
RB              "]"
LC              "{"
RC              "}"
STRUCT          struct
RETURN          return
IF              if
ELSE            else
WHILE           while
%%
{ws}            { /* do nothing*/ }      
{INT}           { yylval = create_lexnode(yylineno, "INT", yytext);
                  return INT;}
{DEC_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal dec number '%s'\n", yylineno, yytext);
                        yylval = create_lexnode(yylineno, "INT", "0");
                        return INT;
                    }
                }
{INT_oct}       { yylval = create_lexnode(yylineno, "INT_oct", yytext);
                  return INT;/* recognize as INT */}
{OCT_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal oct number '%s'\n", yylineno, yytext);
                        yylval = create_lexnode(yylineno, "INT", "00");
                        return INT;
                    }
                }
{INT_hex}       { yylval = create_lexnode(yylineno, "INT_hex", yytext);
                  return INT;/* recognize as INT */}
{HEX_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal hex number '%s'\n", yylineno, yytext);
                        yylval = create_lexnode(yylineno, "INT", "0x0");
                        return INT;
                    }
                }
{FLOAT}         { yylval = create_lexnode(yylineno, "FLOAT", yytext);
                  return FLOAT;}
{FLOAT_exp}     { yylval = create_lexnode(yylineno, "FLOAT_exp", yytext);
                  return FLOAT;}/* recognize as FLOAT */  
{FLOAT_ERROR}   {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal float number '%s'\n", yylineno, yytext);
                        yylval = create_lexnode(yylineno, "FLOAT", "0.0");
                        return FLOAT;
                    }
                } 
{RELOP}         { yylval = create_lexnode(yylineno, "RELOP", yytext); 
                  return RELOP;}
{SEMI}          { yylval = create_lexnode(yylineno, "SEMI", yytext); 
                  return SEMI;}
{COMMA}         { yylval = create_lexnode(yylineno, "COMMA", yytext); 
                  return COMMA;}
{ASSIGNOP}      { yylval = create_lexnode(yylineno, "ASSIGNOP", yytext); 
                  return ASSIGNOP;}
{PLUS}          { yylval = create_lexnode(yylineno, "PLUS", yytext); 
                  return PLUS;}
{MINUS}         { yylval = create_lexnode(yylineno, "MINUS", yytext); 
                  return MINUS;}
{STAR}          { yylval = create_lexnode(yylineno, "STAR", yytext); 
                  return STAR;}
{DIV}           { yylval = create_lexnode(yylineno, "DIV", yytext); 
                  return DIV;}
{AND}           { yylval = create_lexnode(yylineno, "AND", yytext); 
                  return AND;}
{OR}            { yylval = create_lexnode(yylineno, "OR", yytext); 
                  return OR;}
{DOT}           { yylval = create_lexnode(yylineno, "DOT", yytext); 
                  return DOT;}
{NOT}           { yylval = create_lexnode(yylineno, "NOT", yytext); 
                  return NOT;}
{TYPE}          { yylval = create_lexnode(yylineno, "TYPE", yytext); 
                  return TYPE;}
{LP}            { yylval = create_lexnode(yylineno, "LP", yytext); 
                  return LP;}
{RP}            { yylval = create_lexnode(yylineno, "RP", yytext); 
                  return RP;}
{LB}            { yylval = create_lexnode(yylineno, "LB", yytext); 
                  return LB;}
{RB}            { yylval = create_lexnode(yylineno, "RB", yytext); 
                  return RB;}
{LC}            { yylval = create_lexnode(yylineno, "LC", yytext); 
                  return LC;}
{RC}            { yylval = create_lexnode(yylineno, "RC", yytext); 
                  return RC;}
{STRUCT}        { yylval = create_lexnode(yylineno, "STRUCT", yytext); 
                  return STRUCT;}
{RETURN}        { yylval = create_lexnode(yylineno, "RETURN", yytext); 
                  return RETURN;}
{IF}            { yylval = create_lexnode(yylineno, "IF", yytext); 
                  return IF;}
{ELSE}          { yylval = create_lexnode(yylineno, "ELSE", yytext); 
                  return ELSE;}
{WHILE}         { yylval = create_lexnode(yylineno, "WHILE", yytext);
                  return WHILE;}
{ID}            { yylval = create_lexnode(yylineno, "ID", yytext);
                  return ID;}   
{ID_ERROR}      {   if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal id '%s'\n", yylineno, yytext);
                        yylval = create_lexnode(yylineno, "ID", yytext);
                        return ID;
                    }
                }  
"//"            {
                char c = input();
                while (c != '\n'){
                    c = input();
                }
                //printf("\'//\' comment detected, %d characters is skipped\n", skipnum);
                }
"/*"            {
                int firstline = yylineno;
                char c = input(); 
                while (c && (c != EOF)){
                    if (c == '*'){
                        c = input();
                        if (c == '/'){
                            break;
                        } else{
                            unput(c);
                        }
                    }
                    c = input(); 
                    if ((!c) || (c == EOF)){
                        errornum++;
                        printf("Error type A at Line %d: Unmached comment.\n", firstline);    
                    }
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