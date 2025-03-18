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
        Node* nonterminal_node = create_node(pos.first_line, name, name);
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
%right ASSIGNOP  
%left OR 
%left AND 
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left DOT LB RB LP RP
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE
%type <node> Program ExtDefList ExtDef ExtDecList Specifier
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args
%%
Program: ExtDefList                             { $$ = product("Program", @$, 1, $1); root = $$; }
    ;   
ExtDefList: /* empty */                         { $$ = product("ExtDefList", @$, 0); }
    | ExtDef ExtDefList                         { $$ = product("ExtDefList", @$, 2, $1, $2); }
    ;   
ExtDef: Specifier ExtDecList SEMI               { $$ = product("ExtDef", @$, 3, $1, $2, $3); }
    | Specifier SEMI                            { $$ = product("ExtDef", @$, 2, $1, $2); }
    | error SEMI                                { if (errorline != yylineno){  }
    | Specifier FunDec CompSt                   { $$ = product("ExtDef", @$, 3, $1, $2, $3); }
    ;   
ExtDecList: VarDec                              { $$ = product("ExtDecList", @$, 1, $1); }
    | VarDec COMMA ExtDecList                   { $$ = product("ExtDecList", @$, 3, $1, $2, $3); }
    ;           
Specifier: TYPE                                 { $$ = product("Specifier", @$, 1, $1); }
    | StructSpecifier                           { $$ = product("Specifier", @$, 1, $1); }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC    { $$ = product("StructSpecifier", @$, 5, $1, $2, $3, $4, $5); }
    | STRUCT Tag                                { $$ = product("StructSpecifier", @$, 2, $1, $2); }
    ;
OptTag: /* empty */                             { $$ = product("OptTag", @$, 0); }
    | ID                                        { $$ = product("OptTag", @$, 1, $1); }
    ;
Tag: ID                                         { $$ = product("Tag", @$, 1, $1); }
    ;
VarDec: ID                                      { $$ = product("VarDec", @$, 1, $1); }
    | VarDec LB INT RB                          { $$ = product("VarDec", @$, 4, $1, $2, $3, $4); }
    ;
FunDec: ID LP VarList RP                        { $$ = product("FunDec", @$, 4, $1, $2, $3, $4); }
    | ID LP RP                                  { $$ = product("FunDec", @$, 3, $1, $2, $3); }
    ;
VarList: ParamDec COMMA VarList                 { $$ = product("VarList", @$, 3, $1, $2, $3); }
    | ParamDec                                  { $$ = product("VarList", @$, 1, $1); }
    ;
ParamDec: Specifier VarDec                      { $$ = product("ParamDec", @$, 2, $1, $2); }
    ;
CompSt: LC DefList StmtList RC                  { $$ = product("CompSt", @$, 4, $1, $2, $3, $4); }
    ;
StmtList: /* empty */                           { $$ = product("StmtList", @$, 0); }
    | Stmt StmtList                             { $$ = product("StmtList", @$, 2, $1, $2); }
    ;
Stmt: Exp SEMI                                  { $$ = product("Stmt", @$, 2, $1, $2); }
    | CompSt                                    { $$ = product("Stmt", @$, 1, $1); }
    | RETURN Exp SEMI                           { $$ = product("Stmt", @$, 3, $1, $2, $3); }
    | IF LP Exp RP Stmt                         { $$ = product("Stmt", @$, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = product("Stmt", @$, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt                      { $$ = product("Stmt", @$, 5, $1, $2, $3, $4, $5); }
    //| error SEMI                              { onError(INV_S"T); }
    ;
DefList: /* empty */                            { $$ = product("DefList", @$, 0); }
    | Def DefList                               { $$ = product("DefList", @$, 2, $1, $2); }
    ;
Def: Specifier DecList SEMI                     { $$ = product("Def", @$, 3, $1, $2, $3); }
    //| error SEMI                              { onError(INV_D"EF); }
    ;
DecList: Dec                                    { $$ = product("DecList", @$, 1, $1); }
    | Dec COMMA DecList                         { $$ = product("DecList", @$, 3, $1, $2, $3); }
    ;
Dec: VarDec                                     { $$ = product("Dec", @$, 1, $1); }
    | VarDec ASSIGNOP Exp                       { $$ = product("Dec", @$, 3, $1, $2, $3); }
    ;
Exp: Exp ASSIGNOP Exp                           { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp AND Exp                               { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp OR Exp                                { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp RELOP Exp                             { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp PLUS Exp                              { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp MINUS Exp                             { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp STAR Exp                              { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp DIV Exp                               { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | LP Exp RP                                 { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | MINUS Exp %prec NOT                       { $$ = product("Exp", @$, 2, $1, $2); }
    | NOT Exp                                   { $$ = product("Exp", @$, 2, $1, $2); }
    | ID LP Args RP                             { $$ = product("Exp", @$, 4, $1, $2, $3, $4); }
    | ID LP RP                                  { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | Exp LB Exp RB                             { $$ = product("Exp", @$, 4, $1, $2, $3, $4); }
    | Exp DOT ID                                { $$ = product("Exp", @$, 3, $1, $2, $3); }
    | ID                                        { $$ = product("Exp", @$, 1, $1); }
    | INT                                       { $$ = product("Exp", @$, 1, $1); }
    | FLOAT                                     { $$ = product("Exp", @$, 1, $1); }
    ;
Args: Exp COMMA Args                            { $$ = product("Args", @$, 3, $1, $2, $3); }
    | Exp                                       { $$ = product("Args", @$, 1, $1); }
    ;
%%

int yyerror(char const *msg){
    printf("Error type B at line %d: %s\n", yylineno, msg);
}
