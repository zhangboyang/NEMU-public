/* many thanks to
      http://www.blog.chinaunix.net/uid-24774106-id-3566855.html
      http://www.ibm.com/developerworks/cn/linux/l-flexbison.html
   
   C-expression Calculator for FDU-ICS PA
    by Zhang Boyang
*/

%{
#include <stdint.h>
#include <stdio.h>
#define YYSTYPE uint32_t
#include "calc.tab.h"
YYSTYPE get_reg_val(char *name);
int calc_err_flag;
#ifdef CALC_LEX
    YYSTYPE yylval;
#endif
%}

%%

[ \t\r]+        { /* ignore */ }
\n              { return EOL; }
"+"             { return ADD; }
"-"             { return SUB; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"("             { return LP; }
")"             { return RP; }
"<<"            { return LS; }
">>"            { return RS; }
"<"             { return L; }
"<="            { return LE; }
">"             { return G; }
">="            { return GE; }
"=="            { return EQU; }
"!="            { return NEQ; }
"&"             { return AND; }
"^"             { return XOR; }
"|"             { return OR; }
"&&"            { return LAND; }
"||"            { return LOR; }
"?"             { return QMARK; }
":"             { return CMARK; }
"~"             { return NOT; }
"!"             { return LNOT; }
"["             { return SLP; }
"]"             { return SRP; }
[0-9]+                { yylval = atoi(yytext); return NUM; } /* number */
0[xX][a-fA-F0-9]+     { sscanf(yytext, "%x", &yylval); return NUM; } /* hex number */
\$[a-zA-Z]+           { yylval = get_reg_val(yytext); return NUM; } /* register */
\'.\'                 { yylval = (*(yytext + 1)) & 0xff; return NUM; } /* char value */
.                     { calc_err_flag = 1; printf("  error: unknown token %s\n", yytext); }
%%

#ifndef CALC_NEMU
YYSTYPE get_reg_val(char *name)
{
    char *name2 = name;
    YYSTYPE val = 0;
    while (*name2)
        val = val * 3 + *name2++;
    printf("\t%s = %d\n", name, val);
    return val;
}
#endif

#ifdef CALC_LEX

int main()
{
    int token;

    while (token = yylex()) {
        printf("token = %d", token);
        if (token == NUM) printf(", val = %d\n", yylval);
        printf("\n");
    }
    return 0;
}
#endif

