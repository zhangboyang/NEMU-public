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
#define YYERROR_VERBOSE
#ifndef CALC_NEMU
    #define dprintf printf
#else
    #define dprintf(...)
#endif
extern int calc_err_flag;
static YYSTYPE calc_result;
YYSTYPE dref(YYSTYPE x);
%}




%token NUM HNUM

/* operator priority https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B */
%token QMARK CMARK              /*  ?:            */
%token LOR                      /*  ||            */
%token LAND                     /*  &&            */
%token OR                       /*  |             */
%token XOR                      /*  ^             */
%token AND                      /*  &             */
%token EQU NEQ                  /*  == !=         */
%token L LE G GE                /*  < <= > >=     */
%token LS RS                    /*  << >>         */
%token ADD SUB                  /*  + -           */
%token MUL DIV MOD              /*  * / %         */
%token LNOT NOT                 /*  ! ~           */
%token LP RP SLP SRP            /*  ( ) [ ]       */

%token EOL





%%
/*line
  :
  | line expr EOL { printf("=%d\n", $2); }
  ;*/

expr
  : triop_expr { calc_result = $1; }
  ;

triop_expr
  : lor_expr
  | lor_expr QMARK triop_expr CMARK triop_expr { $$ = $1 ? $3 : $5; dprintf("\t%d?%d:%d = %d\n", $1, $3, $5, $$); }
  ;

lor_expr
  : land_expr
  | lor_expr LOR land_expr { $$ = $1 || $3; dprintf("\t%d||%d = %d\n", $1, $3, $$); }
  ;

land_expr
  : or_expr
  | land_expr LAND or_expr { $$ = $1 && $3; dprintf("\t%d&&%d = %d\n", $1, $3, $$); }
  ;

or_expr
  : xor_expr
  | or_expr OR xor_expr { $$ = $1 | $3; dprintf("\t%d|%d = %d\n", $1, $3, $$); }
  ;

xor_expr
  : and_expr
  | xor_expr XOR and_expr { $$ = $1 ^ $3; dprintf("\t%d^%d = %d\n", $1, $3, $$); }
  ;

and_expr
  : eqcmp_expr
  | and_expr AND eqcmp_expr { $$ = $1 & $3; dprintf("\t%d&%d = %d\n", $1, $3, $$); }
  ;

eqcmp_expr
  : lgcmp_expr
  | eqcmp_expr EQU lgcmp_expr { $$ = $1 == $3; dprintf("\t%d==%d = %d\n", $1, $3, $$); }
  | eqcmp_expr NEQ lgcmp_expr { $$ = $1 != $3; dprintf("\t%d!=%d = %d\n", $1, $3, $$); }
  ;

lgcmp_expr
  : lshift_rshift_expr
  | lgcmp_expr L lshift_rshift_expr  { $$ = $1 < $3;  dprintf("\t%d<%d = %d\n", $1, $3, $$); }
  | lgcmp_expr LE lshift_rshift_expr { $$ = $1 <= $3; dprintf("\t%d<=%d = %d\n", $1, $3, $$); }
  | lgcmp_expr G lshift_rshift_expr  { $$ = $1 > $3;  dprintf("\t%d>%d = %d\n", $1, $3, $$); }
  | lgcmp_expr GE lshift_rshift_expr { $$ = $1 >= $3; dprintf("\t%d>=%d = %d\n", $1, $3, $$); }
  ;

lshift_rshift_expr
  : add_sub_expr
  | lshift_rshift_expr LS add_sub_expr { $$ = $1 << $3; dprintf("\t%d<<%d = %d\n", $1, $3, $$); }
  | lshift_rshift_expr RS add_sub_expr { $$ = $1 >> $3; dprintf("\t%d>>%d = %d\n", $1, $3, $$); }
  ;

add_sub_expr
  : mul_div_expr
  | add_sub_expr ADD mul_div_expr { $$ = $1 + $3; dprintf("\t%d+%d = %d\n", $1, $3, $$); }
  | add_sub_expr SUB mul_div_expr { $$ = $1 - $3; dprintf("\t%d-%d = %d\n", $1, $3, $$); }
  ;

mul_div_expr
  : unary_expr
  | mul_div_expr MUL unary_expr { $$ = $1 * $3; dprintf("\t%d*%d = %d\n", $1, $3, $$); }
  | mul_div_expr DIV unary_expr { $$ = $1 / $3; dprintf("\t%d/%d = %d\n", $1, $3, $$); }
  | mul_div_expr MOD unary_expr { $$ = $1 % $3; dprintf("\t%d%%%d = %d\n", $1, $3, $$); }
  ;

unary_expr
  : subscript_expr
  | NOT unary_expr { $$ = ~$2; dprintf("\t~%d = %d\n", $2, $$); }
  | LNOT unary_expr { $$ = !$2; dprintf("\t!%d = %d\n", $2, $$); }
  | ADD unary_expr { $$ = +$2; dprintf("\t+%d = %d\n", $2, $$); } /* unary + */
  | SUB unary_expr { $$ = -$2; dprintf("\t-%d = %d\n", $2, $$); } /* unary - */
  | MUL unary_expr { $$ = dref($2); dprintf("\t*%d = %d\n", $2, $$); } /* dereference */
  ;
  
subscript_expr
  : operand_expr
  | subscript_expr SLP expr SRP { $$ = dref($1 + $3 * 4); dprintf("\t%d[%d] = *(%d+%d*4) = %d\n", $1, $3, $1, $3, $$); }
  ;

operand_expr
  : number_expr
  | LP expr RP { $$ = $2; }
  ;

number_expr
  : NUM
  ;

%%




int yyerror(const char *s)
{
    calc_err_flag = 1;
    printf("  error: %s\n", s);
}

YYSTYPE calc(char *buf, int *success_flag)
{
    calc_err_flag = 0;
    yy_switch_to_buffer(yy_scan_string(buf));
    yyparse();
    *success_flag = !calc_err_flag;
    return calc_result;
}


#ifndef CALC_NEMU

YYSTYPE dref(YYSTYPE x)
{
    return x * 10;
}

int main()
{
    char buf[4096];
    int ans;
    int flag;
    while (1) {
        scanf("%s", buf);
        ans = calc(buf, &flag);
        printf("ans=%d flag=%d\n", ans, flag);
    }
}

#endif
