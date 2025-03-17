%{
 #include <stdio.h>
 int chars = 0;
 int words = 0;
 int lines = 0;
%}

letter      [_a-zA-Z]
digit       [0-9]

%%
{letter}+   {words++; chars += yyleng;}
\n  {chars++; lines++;}
.   {chars++;}
{digit}+    {printf("%d is a digit", atoi(yytext));}
%%
int main(int argc, char** argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    printf("lines:%8d words:%8d chars:%8d\n",lines, words, chars);
    return 0;
}

