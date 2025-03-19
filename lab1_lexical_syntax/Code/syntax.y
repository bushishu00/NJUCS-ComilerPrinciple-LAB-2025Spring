%{
    #include "lex.yy.c"
    #include "mynode.h"

    #include <stdio.h>
    #include <stdlib.h>

    extern int yylineno;
    extern Node* root;
    extern int errorline;
    extern int errornum;

    Node* product(char* name, YYLTYPE pos, int cnt, ...){
        /* Node* create_node(int lineno, char* name, char* value); */
        Node* nonterminal_node = create_node(pos.first_line, name, name, SYN_NODE);
        va_list children;
        va_start(children, cnt);
        for(int i = 0; i < cnt; i++){
            Node* child = va_arg(children, Node*);
            add_child(nonterminal_node, child);
        }
        va_end(children);
        return nonterminal_node;
    }
%}
%union {
    Node* node;
}
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV 
%token <node> AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%left OR 
%left AND 
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%right ASSIGNOP  

%left DOT LB RB LP RP
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE
%type <node> Program ExtDefList ExtDef ExtDecList Specifier
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args
%%
Program: ExtDefList                             { $$ = product("Program", @$, 1, $1); root = $$; }/* impossible for any error */
    ;   
ExtDefList: /* empty */                         { $$ = product("ExtDefList", @$, 0); }/* no external  */
    | ExtDef ExtDefList                         { $$ = product("ExtDefList", @$, 2, $1, $2); }/* int a; int b; ... */
    ;   
ExtDef: Specifier ExtDecList SEMI               { $$ = product("ExtDef", @$, 3, $1, $2, $3); }/* int a,b,c; */
    | Specifier SEMI                            { $$ = product("ExtDef", @$, 2, $1, $2); }/* int; it's strange but correct anyway */
    | Specifier FunDec CompSt                   { $$ = product("ExtDef", @$, 3, $1, $2, $3); }/* int main(...) {...} */
    | error SEMI                                { yyerror("syntax error1"); }/* double x; */
    //| Specifier error SEMI                      { yyerror("syntax error2"); }/* int a=; */
    | error Specifier SEMI                      { yyerror("syntax error3"); }
    //| Specifier ExtDecList error                { yyerror("expected \';\'"); }
    //| Specifier error                           { yyerror("expected \';\'"); }
    ;   
ExtDecList: VarDec                              { $$ = product("ExtDecList", @$, 1, $1); }/* a */
    | VarDec COMMA ExtDecList                   { $$ = product("ExtDecList", @$, 3, $1, $2, $3); }/* a,b */
    | VarDec error ExtDecList                   { yyerror("syntax error4"); }/* a[,b */
    ;           
Specifier: TYPE                                 { $$ = product("Specifier", @$, 1, $1); }/* int/float */
    | StructSpecifier                           { $$ = product("Specifier", @$, 1, $1); }/* struct... */
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    { $$ = product("StructSpecifier", @$, 5, $1, $2, $3, $4, $5); }/* struct {...}  or  struct ID {...} */
    | STRUCT Tag                                { $$ = product("StructSpecifier", @$, 2, $1, $2); }/* struct ID */
    ;
OptTag: /* empty */                             { $$ = product("OptTag", @$, 0); }
    | ID                                        { $$ = product("OptTag", @$, 1, $1); }/* error ID is handled in lex */
    ;
Tag: ID                                         { $$ = product("Tag", @$, 1, $1); }/* error ID is handled in lex */
    ;
VarDec: ID                                      { $$ = product("VarDec", @$, 1, $1); }/* error ID is handled in lex */
    | VarDec LB INT RB                          { $$ = product("VarDec", @$, 4, $1, $2, $3, $4); }/* array, e.g. a[123] */
    | VarDec LB error RB                        { yyerror("syntax error5"); }/* a[a,b] a[3][] */
    ;
FunDec: ID LP VarList RP                        { $$ = product("FunDec", @$, 4, $1, $2, $3, $4); }/* main(int a,...) */
    | ID LP RP                                  { $$ = product("FunDec", @$, 3, $1, $2, $3); }/* main() */
    | ID LP error RP                            { yyerror("syntax error6"); }/* main( a ) actually it's a Varlist error*/
    | error LP VarList RP                       { yyerror("syntax error7"); }
    ;
VarList: ParamDec COMMA VarList                 { $$ = product("VarList", @$, 3, $1, $2, $3); }/* int a, intb, .... */
    | ParamDec                                  { $$ = product("VarList", @$, 1, $1); }/* int a */
    ;
ParamDec: Specifier VarDec                      { $$ = product("ParamDec", @$, 2, $1, $2); }/* int a */
    ;
CompSt: LC DefList StmtList RC                  { $$ = product("CompSt", @$, 4, $1, $2, $3, $4); }/* {int a; a = 2;} */
    //| error RC                                  { yyerror("syntax error"); }
    ;
StmtList: /* empty */                           { $$ = product("StmtList", @$, 0); }/*  */
    | Stmt StmtList                             { $$ = product("StmtList", @$, 2, $1, $2); }/* a = 1; b = 2; ...  */
    ;
Stmt: Exp SEMI                                  { $$ = product("Stmt", @$, 2, $1, $2); }/*  */
    | CompSt                                    { $$ = product("Stmt", @$, 1, $1); }/* {...} */
    | RETURN Exp SEMI                           { $$ = product("Stmt", @$, 3, $1, $2, $3); }/* return x==1; */
    | IF LP Exp RP Stmt                         { $$ = product("Stmt", @$, 5, $1, $2, $3, $4, $5); }/* if (a==1) ... */
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = product("Stmt", @$, 7, $1, $2, $3, $4, $5, $6, $7); }/* if (a==1) ... else ... */
    | WHILE LP Exp RP Stmt                      { $$ = product("Stmt", @$, 5, $1, $2, $3, $4, $5); }/* while(a>1) ... */
    //| error SEMI                                { yyerror("syntax error8"); }/* a++; */
    | Exp error SEMI                            { yyerror("syntax error9"); }
    | RETURN Exp error                          { yyerror("syntax error10"); }/* return a */
    | RETURN error SEMI                         { yyerror("syntax error11"); }/* return a++; */
    ;
DefList: /* empty */                            { $$ = product("DefList", @$, 0); }/*  */
    | Def DefList                               { $$ = product("DefList", @$, 2, $1, $2); }/* int a; int b; ... */
    ;
Def: Specifier DecList SEMI                     { $$ = product("Def", @$, 3, $1, $2, $3); }/* int a; */
    | Specifier error SEMI                      { yyerror("syntax error12"); }/* */
    | Specifier DecList error                   { yyerror("syntax error13"); }/*int a int b;  */
    ;
DecList: Dec                                    { $$ = product("DecList", @$, 1, $1); }/* a */
    | Dec COMMA DecList                         { $$ = product("DecList", @$, 3, $1, $2, $3); }/* a,b,c */
    ;
Dec:  VarDec                                    { $$ = product("Dec", @$, 1, $1); }/* a */
    | VarDec ASSIGNOP Exp                       { $$ = product("Dec", @$, 3, $1, $2, $3); }/* a = 3>1 */
    ;
Exp:  Exp ASSIGNOP Exp                          { $$ = product("Exp", @$, 3, $1, $2, $3); }/* a+b > c+d */
    | Exp AND Exp                               { $$ = product("Exp", @$, 3, $1, $2, $3); }/* a+b & c+d */
    | Exp OR Exp                                { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | Exp RELOP Exp                             { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | Exp PLUS Exp                              { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | Exp MINUS Exp                             { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | Exp STAR Exp                              { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | Exp DIV Exp                               { $$ = product("Exp", @$, 3, $1, $2, $3); }/* similarly */
    | LP Exp RP                                 { $$ = product("Exp", @$, 3, $1, $2, $3); }/* (a+b) */
    | MINUS Exp %prec NOT                       { $$ = product("Exp", @$, 2, $1, $2); }/* -a + b */
    | NOT Exp                                   { $$ = product("Exp", @$, 2, $1, $2); }/* !a */
    | ID LP Args RP                             { $$ = product("Exp", @$, 4, $1, $2, $3, $4); }/* printf("hello") */
    | ID LP RP                                  { $$ = product("Exp", @$, 3, $1, $2, $3); }/* printf() */
    | Exp LB Exp RB                             { $$ = product("Exp", @$, 4, $1, $2, $3, $4); }/* a[i] */
    | Exp DOT ID                                { $$ = product("Exp", @$, 3, $1, $2, $3); }/* node.child.name */
    | ID                                        { $$ = product("Exp", @$, 1, $1); }/* a */
    | INT                                       { $$ = product("Exp", @$, 1, $1); }/* 1 */
    | FLOAT                                     { $$ = product("Exp", @$, 1, $1); }/* 1.0 */
    | Exp ASSIGNOP error                        { yyerror("syntax error14"); }
    | Exp AND error                             { yyerror("syntax error15"); }
    | Exp OR error                              { yyerror("syntax error16"); }
    | Exp RELOP error                           { yyerror("syntax error17"); }
    | Exp PLUS error                            { yyerror("syntax error18"); }
    | Exp MINUS error                           { yyerror("syntax error19"); }
    | Exp STAR error                            { yyerror("syntax error20"); }
    | Exp DIV error                             { yyerror("syntax error21"); }
    | LP error RP                               { yyerror("syntax error22"); }
    | MINUS error                               { yyerror("syntax error23"); }
    | NOT error                                 { yyerror("syntax error24"); }
    | ID LP error RP                            { yyerror("syntax error25"); }
    | Exp LB error RB                           { yyerror("syntax error26"); }
    //| error                                     { yyerror("Expression syntax error.");}
    ;
Args: Exp COMMA Args                            { $$ = product("Args", @$, 3, $1, $2, $3); }/* a+3,root */
    | Exp                                       { $$ = product("Args", @$, 1, $1); }/* a */
    ;
%%

int yyerror(char const *msg){
    errornum ++;
    printf("Error type B at Line %d: %s\n", yylineno, msg);
}
