%{
    #include "lex.yy.c"
    #include "astnode.h"

    #include <stdio.h>
    #include <stdlib.h>

    extern int yylineno;
    extern astNode* root;
    extern int errorline;
    extern int errornum;
    void yyerror(const char *);
%}

%token INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND 
%token OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%type Program ExtDefList ExtDef ExtDecList
%type Specifier StructSpecifier OptTag Tag
%type VarDec FunDec VarList ParamDec
%type CompSt StmtList Stmt
%type DefList Def DecList Dec
%type Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT NEG
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program: 
    ExtDefList                                  { $$ = create_node(SYN_NODE, @$.first_line, "Program", "Program", 1, $1); root = $$; }/* impossible for any error */
    ;   
ExtDefList: 
    ExtDef ExtDefList                           { $$ = create_node(SYN_NODE, @$.first_line, "ExtDefList", "ExtDefList", 2, $1, $2); }/* int a; int b; ... */
    | /* empty */                               { $$ = NULL; }/* no external  */
    ;   
ExtDef: 
    Specifier ExtDecList SEMI                   { $$ = create_node(SYN_NODE, @$.first_line, "ExtDef", "ExtDef", 3, $1, $2, $3); }/* int a,b,c; */
    | Specifier SEMI                            { $$ = create_node(SYN_NODE, @$.first_line, "ExtDef", "ExtDef", 2, $1, $2); }/* int; it's strange but correct anyway */
    | Specifier FunDec CompSt                   { $$ = create_node(SYN_NODE, @$.first_line, "ExtDef", "ExtDef", 3, $1, $2, $3); }/* int main(...) {...} */
    | Specifier FunDec SEMI                     { $$ = create_node(SYN_NODE, @$.first_line, "ExtDef", "ExtDef", 3, $1, $2, $3); }/* int main(...) */
    //| error SEMI                                { errornum++; }/* double x; */
    | Specifier error SEMI                      { errornum++; }/* int a=; */
    //| error Specifier SEMI                      { errornum++; }
    //| Specifier ExtDecList error                { yyerror("expected \';\'"); }
    //| Specifier error                           { yyerror("expected \';\'"); }
    ;   
ExtDecList: 
    VarDec                                      { $$ = create_node(SYN_NODE, @$.first_line, "ExtDecList", "ExtDecList", 1, $1); }/* a */
    | VarDec COMMA ExtDecList                   { $$ = create_node(SYN_NODE, @$.first_line, "ExtDecList", "ExtDecList", 3, $1, $2, $3); }/* a,b */
    //| VarDec error ExtDecList                   { errornum++; }/* a[,b */
    ;           
Specifier: 
    TYPE                                        { $$ = create_node(SYN_NODE, @$.first_line, "Specifier", "Specifier", 1, $1); }/* int/float */
    | StructSpecifier                           { $$ = create_node(SYN_NODE, @$.first_line, "Specifier", "Specifier", 1, $1); }/* struct... */
    ;
StructSpecifier: 
    STRUCT OptTag LC DefList RC                 { $$ = create_node(SYN_NODE, @$.first_line, "StructSpecifier", "StructSpecifier", 5, $1, $2, $3, $4, $5); }/* struct {...}  or  struct ID {...} */
    | STRUCT Tag                                { $$ = create_node(SYN_NODE, @$.first_line, "StructSpecifier", "StructSpecifier", 2, $1, $2); }/* struct ID */
    | error                                     { errornum++; }
    ;
OptTag: 
    ID                                          { $$ = create_node(SYN_NODE, @$.first_line, "OptTag", "OptTag", 1, $1); }/* error ID is handled in lex */
    | /* empty */                               { $$ = NULL; }
    ;
Tag: 
    ID                                          { $$ = create_node(SYN_NODE, @$.first_line, "Tag", "Tag", 1, $1); }/* error ID is handled in lex */
    ;


VarDec: 
    ID                                          { $$ = create_node(SYN_NODE, @$.first_line, "VarDec", "VarDec", 1, $1); }/* error ID is handled in lex */
    | VarDec LB INT RB                          { $$ = create_node(SYN_NODE, @$.first_line, "VarDec", "VarDec", 4, $1, $2, $3, $4); }/* array, e.g. a[123] */
    //| VarDec LB error RB                        { errornum++; }/* a[a,b] a[3][] */
    ;
FunDec: 
    ID LP VarList RP                            { $$ = create_node(SYN_NODE, @$.first_line, "FunDec", "FunDec", 4, $1, $2, $3, $4); }/* main(int a,...) */
    | ID LP RP                                  { $$ = create_node(SYN_NODE, @$.first_line, "FunDec", "FunDec", 3, $1, $2, $3); }/* main() */
    | ID LP error RP                            { errornum++; }/* main( a ) actually it's a Varlist error*/
    | error LP VarList RP                       { errornum++; }
    ;
VarList: 
    ParamDec COMMA VarList                      { $$ = create_node(SYN_NODE, @$.first_line, "VarList", "VarList", 3, $1, $2, $3); }/* int a, intb, .... */
    | ParamDec                                  { $$ = create_node(SYN_NODE, @$.first_line, "VarList", "VarList", 1, $1); }/* int a */
    ;
ParamDec: 
    Specifier VarDec                            { $$ = create_node(SYN_NODE, @$.first_line, "ParamDec", "ParamDec", 2, $1, $2); }/* int a */
    ;
CompSt: 
    LC DefList StmtList RC                      { $$ = create_node(SYN_NODE, @$.first_line, "CompSt", "CompSt", 4, $1, $2, $3, $4); }/* {int a; a = 2;} */
    | error RC                                  { errornum++; }
    ;
StmtList: 
    Stmt StmtList                               { $$ = create_node(SYN_NODE, @$.first_line, "StmtList", "StmtList", 2, $1, $2); }/* a = 1; b = 2; ...  */
    | /* empty */                               { $$ = NULL; }/*  */
    ;
Stmt: 
    Exp SEMI                                    { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 2, $1, $2); }/*  */
    //| Exp error                                 { errornum++; }
    | CompSt                                    { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 1, $1); }/* {...} */
    | RETURN Exp SEMI                           { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 3, $1, $2, $3); }/* return x==1; */
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 5, $1, $2, $3, $4, $5); }/* if (a==1) ... */
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }/* if (a==1) ... else ... */
    | WHILE LP Exp RP Stmt                      { $$ = create_node(SYN_NODE, @$.first_line, "Stmt", "Stmt", 5, $1, $2, $3, $4, $5); }/* while(a>1) ... */
    //| Exp error SEMI                            { errornum++; }
    //| RETURN Exp error                          { errornum++; }/* return a */
    //| RETURN error SEMI                         { errornum++; }/* return a++; */
    | error SEMI                                { errornum++; }/* a++; */    
    ;


DefList: 
    Def DefList                                 { $$ = create_node(SYN_NODE, @$.first_line, "DefList", "DefList", 2, $1, $2); }/* int a; int b; ... */
    | /* empty */                               { $$ = NULL; }/*  */
    ;
Def: 
    Specifier DecList SEMI                      { $$ = create_node(SYN_NODE, @$.first_line, "Def", "Def", 3, $1, $2, $3); }/* int a; */
    | Specifier error SEMI                      { errornum++; }/* */
    //| Specifier DecList error                   { errornum++; }/*int a int b;  */
    ;
DecList: 
    Dec                                         { $$ = create_node(SYN_NODE, @$.first_line, "DecList", "DecList", 1, $1); }/* a */
    | Dec COMMA DecList                         { $$ = create_node(SYN_NODE, @$.first_line, "DecList", "DecList", 3, $1, $2, $3); }/* a,b,c */
    ;
Dec:  
    VarDec                                      { $$ = create_node(SYN_NODE, @$.first_line, "Dec", "Dec", 1, $1); }/* a */
    | VarDec ASSIGNOP Exp                       { $$ = create_node(SYN_NODE, @$.first_line, "Dec", "Dec", 3, $1, $2, $3); }/* a = 3>1 */
    ;
Exp:  
    Exp ASSIGNOP Exp                            { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* a+b > c+d */
    | Exp AND Exp                               { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* a+b & c+d */
    | Exp OR Exp                                { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | Exp RELOP Exp                             { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | Exp PLUS Exp                              { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | Exp MINUS Exp                             { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | Exp STAR Exp                              { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | Exp DIV Exp                               { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* similarly */
    | LP Exp RP                                 { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* (a+b) */
    | MINUS Exp %prec NEG                       { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 2, $1, $2); }/* -a + b */
    | NOT Exp                                   { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 2, $1, $2); }/* !a */
    | ID LP Args RP                             { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 4, $1, $2, $3, $4); }/* printf("hello") */
    | ID LP RP                                  { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* printf() */
    | Exp LB Exp RB                             { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 4, $1, $2, $3, $4); }/* a[i] */
    //| Exp LB error RB                           { errornum++; }
    | Exp DOT ID                                { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 3, $1, $2, $3); }/* node.child.name */
    //| error DOT ID                              { errornum++; }
    | ID                                        { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 1, $1); }/* a */
    | INT                                       { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 1, $1); }/* 1 */
    | FLOAT                                     { $$ = create_node(SYN_NODE, @$.first_line, "Exp", "Exp", 1, $1); }/* 1.0 */
    | error                                     { errornum++; }
    ;
Args: 
    Exp COMMA Args                              { $$ = create_node(SYN_NODE, @$.first_line, "Args", "Args", 3, $1, $2, $3); }/* a+3,root */
    | Exp                                       { $$ = create_node(SYN_NODE, @$.first_line, "Args", "Args", 1, $1); }/* a */
    ;
%%

void yyerror(char const *msg){
    if (errorline != yylineno){
        errorline = yylineno;
        printf("Error type B at Line %d: %s near %s.\n", yylineno, msg, yytext);
    }
}

/* When compiling without Makefile, the below fundef is needed */
int yywrap() 
{ 
   return 1; 
}