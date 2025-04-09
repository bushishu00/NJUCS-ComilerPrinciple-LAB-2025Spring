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
{INT}           { yylval = create_node(LEX_NODE, yylineno, "INT", yytext, 0);
                  return INT;}
{INT_oct}       { yylval = create_node(LEX_NODE, yylineno, "INT", yytext, 0);
                  return INT;/* recognize as INT */}
{OCT_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal oct number '%s'\n", yylineno, yytext);
                        yylval = create_node(LEX_NODE, yylineno, "INT", "00", 0);
                        return INT;
                    }
                }
{INT_hex}       { yylval = create_node(LEX_NODE, yylineno, "INT", yytext, 0);
                  return INT;/* recognize as INT */}
{HEX_ERROR}     {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal hex number '%s'\n", yylineno, yytext);
                        yylval = create_node(LEX_NODE, yylineno, "INT", "0x0", 0);
                        return INT;
                    }
                }
{FLOAT}         { yylval = create_node(LEX_NODE, yylineno, "FLOAT", yytext, 0);
                  return FLOAT;}
{FLOAT_exp}     { yylval = create_node(LEX_NODE, yylineno, "FLOAT", yytext, 0);
                  return FLOAT;}/* recognize as FLOAT */  
{FLOAT_ERROR}   {
                    if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal float number '%s'\n", yylineno, yytext);
                        yylval = create_node(LEX_NODE, yylineno, "FLOAT", "0.0", 0);
                        return FLOAT;
                    }
                } 
{RELOP}         { yylval = create_node(LEX_NODE, yylineno, "RELOP", yytext, 0); 
                  return RELOP;}
{SEMI}          { yylval = create_node(LEX_NODE, yylineno, "SEMI", yytext, 0); 
                  return SEMI;}
{COMMA}         { yylval = create_node(LEX_NODE, yylineno, "COMMA", yytext, 0); 
                  return COMMA;}
{ASSIGNOP}      { yylval = create_node(LEX_NODE, yylineno, "ASSIGNOP", yytext, 0); 
                  return ASSIGNOP;}
{PLUS}          { yylval = create_node(LEX_NODE, yylineno, "PLUS", yytext, 0); 
                  return PLUS;}
{MINUS}         { yylval = create_node(LEX_NODE, yylineno, "MINUS", yytext, 0); 
                  return MINUS;}
{STAR}          { yylval = create_node(LEX_NODE, yylineno, "STAR", yytext, 0); 
                  return STAR;}
{DIV}           { yylval = create_node(LEX_NODE, yylineno, "DIV", yytext, 0); 
                  return DIV;}
{AND}           { yylval = create_node(LEX_NODE, yylineno, "AND", yytext, 0); 
                  return AND;}
{OR}            { yylval = create_node(LEX_NODE, yylineno, "OR", yytext, 0); 
                  return OR;}
{DOT}           { yylval = create_node(LEX_NODE, yylineno, "DOT", yytext, 0); 
                  return DOT;}
{NOT}           { yylval = create_node(LEX_NODE, yylineno, "NOT", yytext, 0); 
                  return NOT;}
{TYPE}          { yylval = create_node(LEX_NODE, yylineno, "TYPE", yytext, 0); 
                  return TYPE;}
{LP}            { yylval = create_node(LEX_NODE, yylineno, "LP", yytext, 0); 
                  return LP;}
{RP}            { yylval = create_node(LEX_NODE, yylineno, "RP", yytext, 0); 
                  return RP;}
{LB}            { yylval = create_node(LEX_NODE, yylineno, "LB", yytext, 0); 
                  return LB;}
{RB}            { yylval = create_node(LEX_NODE, yylineno, "RB", yytext, 0); 
                  return RB;}
{LC}            { yylval = create_node(LEX_NODE, yylineno, "LC", yytext, 0); 
                  return LC;}
{RC}            { yylval = create_node(LEX_NODE, yylineno, "RC", yytext, 0); 
                  return RC;}
{STRUCT}        { yylval = create_node(LEX_NODE, yylineno, "STRUCT", yytext, 0); 
                  return STRUCT;}
{RETURN}        { yylval = create_node(LEX_NODE, yylineno, "RETURN", yytext, 0); 
                  return RETURN;}
{IF}            { yylval = create_node(LEX_NODE, yylineno, "IF", yytext, 0); 
                  return IF;}
{ELSE}          { yylval = create_node(LEX_NODE, yylineno, "ELSE", yytext, 0); 
                  return ELSE;}
{WHILE}         { yylval = create_node(LEX_NODE, yylineno, "WHILE", yytext, 0);
                  return WHILE;}
{ID}            { yylval = create_node(LEX_NODE, yylineno, "ID", yytext, 0);
                  return ID;}   
{ID_ERROR}      {   if (errorline != yylineno){
                        errorline = yylineno;
                        errornum++;
                        printf("Error type A at Line %d: Illegal id '%s'\n", yylineno, yytext);
                        yylval = create_node(LEX_NODE, yylineno, "ID", yytext, 0);
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