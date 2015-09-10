#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

enum {
	NOTYPE = 256,

	/* DONE: Add more token types */
     QMARK, CMARK,              /*  ?:            */
     TRIOP,
     
     LOR,                      /*  ||            */
     LAND,                     /*  &&            */
     OR,                       /*  |             */
     XOR,                      /*  ^             */
     AND,                      /*  &             */
     EQU, NEQ,                  /*  == !=         */
     L, LE, G, GE,                /*  < <= > >=     */
     LS, RS,                    /*  << >>         */
     ADD, SUB,                  /*  + -           */
     MUL, DIV, MOD,              /*  * / %         */
     LNOT, NOT,                 /*  ! ~           */
     LP, RP, SLP, SRP,            /*  ( ) [ ]       */
     SUBSCRIPT,
     
     UADD, USUB, DREF,
     
     DEC_NUM, HEX_NUM, REG_NUM, SYM_NUM, CHAR_NUM,
     NUM,
     
     UNKNOWN_TOKEN,
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* DONE: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{"[ \t]+",	NOTYPE},				// spaces
//	{"\\+", '+'},					// plus
//	{"==", EQ}						// equal

    /* two-char operator */
    {"<<", LS},
    {">>", RS},
    {"<=", LE},
    {">=", GE},
    {"==", EQU},
    {"!=", NEQ},
    {"&&", LAND},
    {"\\|\\|", LOR},
    
    /* single-char operator */
    {"\\+", ADD},
    {"-", SUB},
    {"\\*", MUL},
    {"/", DIV},
    {"%", MOD},
    {"\\(", LP},
    {"\\)", RP},
    {"<", L},
    {">", G},
    {"&", AND},
    {"\\^", XOR},
    {"\\|", OR},
    {"\\?", QMARK},
    {":", CMARK},
    {"~", NOT},
    {"!", LNOT},
    {"\\[", SLP},
    {"\\]", SRP},
    
    {"0[xX][a-fA-F0-9]+", HEX_NUM},//     { sscanf(yytext, "%x", &yylval); return NUM; } /* hex number */
    {"[0-9]+", DEC_NUM},//              { yylval = atoi(yytext); return NUM; } /* number */
    {"\\$[a-zA-Z]+", REG_NUM},//           { yylval = get_reg_val(yytext); return NUM; } /* register */
    {"[a-zA-Z0-9_]+", SYM_NUM},//           { yylval = get_reg_val(yytext); return NUM; } /* symbol */
    {"'.'", CHAR_NUM},//                 { yylval = *(yytext + 1); return NUM; } /* char value */
    {".", UNKNOWN_TOKEN},//                     { calc_err_flag = 1; printf("  error: unknown token %s\n", yytext); }

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];
static int init_regex_flag = 0;

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
	
	init_regex_flag = 1;
}

#define MAX_TOKENS 256
typedef struct token {
	int type;
	uint32_t *addr;
} Token;

Token tokens[MAX_TOKENS];
int nr_token;

#define MAX_EXPR_MEM MAX_TOKENS
static uint32_t *expr_mem;
static int nr_addr;

typedef struct opcode {
    int type;
    uint32_t *dest, *op1, *op2, *op3;
} Opcode;

#define MAX_OPCODES MAX_TOKENS
Opcode *opcodes;
static int nr_opcode;

uint32_t *final_result;

static void prepare_compile()
{
    expr_mem = malloc(MAX_EXPR_MEM * sizeof(uint32_t));
    opcodes = malloc(MAX_OPCODES * sizeof(Opcode));
    nr_addr = 0;
    nr_opcode = 0;
}

static void clean_compile()
{
    free(opcodes);
    free(expr_mem);
}

static uint32_t *alloc_num_addr(uint32_t val)
{
    assert(nr_addr < MAX_EXPR_MEM);
    expr_mem[nr_addr] = val;
    return &expr_mem[nr_addr++];
}

static uint32_t *alloc_addr()
{
    return &expr_mem[nr_addr++];
}


static uint32_t cpu_registers[16];
static void load_registers()
{
    int i;
    for (i = 0; i < 8; i++)
        cpu_registers[i] = reg_w(i);
    for (i = 0; i < 8; i++)
        cpu_registers[i + 8] = reg_b(i);
}

static uint32_t *alloc_reg_addr(char *reg_name, int reg_name_len) // if reg_name illegal, return NULL
{
    int i;
    char buf[5];
    if (reg_name_len > 4) return NULL;
    strncpy(buf, reg_name, sizeof(buf));
    buf[sizeof(buf) - 1] = 0;
    
    for (i = 1; i < strlen(buf); i++)
        buf[i] = tolower(buf[i]);
    
    if (strcmp(buf + 1, "eip") == 0)
        return &cpu.eip;
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsl[i]) == 0)
            return &reg_l(i);
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsw[i]) == 0)
            return &cpu_registers[i];
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsb[i]) == 0)
            return &cpu_registers[i + 8];
    return NULL;
}

static uint32_t *alloc_sym_addr(char *sym_name, int sym_name_len) // if sym_name illegal, return NULL
{
    extern uint32_t *get_symval_addr(char *sym_name, int sym_name_len);
    return get_symval_addr(sym_name, sym_name_len);
}

static bool make_token(char *e) {
    //printf("make_token(%s)\n", e);
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;
	e = strdup(e); // copy e to ensure we can write to e

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
		    //printf("tring %d : %s\n", i, rules[i].regex);
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				
				//printf("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* DONE: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */
				int tt = rules[i].token_type;
				
				if (tt == NOTYPE) break;
				if (tt == UNKNOWN_TOKEN) {
		            printf(c_red "  unregonized token %.*s" c_normal "\n", substr_len, substr_start);
		            goto fail;
		        }
		        
		        tokens[nr_token].type = tt;
		        
		        if (tt == DEC_NUM || tt == HEX_NUM || tt == CHAR_NUM) {
			        uint32_t val;
			        char sv = substr_start[substr_len]; // make it become a string
			        substr_start[substr_len] = '\0';
			        if (tt == DEC_NUM)
			            val = atoi(substr_start);
			        else if (tt == HEX_NUM)
				        sscanf(substr_start, "%x", &val);
				    else if (tt == CHAR_NUM)
				        val = (*(substr_start + 1)) & 0xff;
				    else assert(0);
				    substr_start[substr_len] = sv;
				    
				    tokens[nr_token].addr = alloc_num_addr(val);
				    tokens[nr_token].type = NUM;
			    }
			    
			    if (tt == REG_NUM) {
			        if (!(tokens[nr_token].addr = alloc_reg_addr(substr_start, substr_len))) {
			            printf(c_red "  unregonized register %.*s" c_normal "\n", substr_len, substr_start);
		                goto fail;
			        }
			        tokens[nr_token].type = NUM;
			    }
			    
			    if (tt == SYM_NUM) {
			        if (!(tokens[nr_token].addr = alloc_sym_addr(substr_start, substr_len))) {
			            printf(c_red "  unregonized symbol %.*s" c_normal "\n", substr_len, substr_start);
		                goto fail;
			        }
			        tokens[nr_token].type = NUM;
			    }
			    
			    nr_token++;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			goto fail;
		}
	}

    free(e);
	return true;
fail:
    free(e);
    return false;
}



#define make_eval_opcode_1(TYPE, OP) \
    case TYPE: *op->dest = OP (*op->op1); break;
#define make_eval_opcode_2(TYPE, OP) \
    case TYPE: *op->dest = (*op->op1) OP (*op->op2); break;
#define make_eval_opcode_3(TYPE) \
    case TYPE: *op->dest = (*op->op1) ? (*op->op2) : (*op->op3); break;
void eval_opcode(Opcode *op)
{
    switch(op->type) {
        case DREF:
            *op->dest = safe_swaddr_read(*op->op1, 4, NULL, R_DS);
            break;
        case SUBSCRIPT:
            *op->dest = safe_swaddr_read(*op->op1 + *op->op2 * 4, 4, NULL, R_DS);
            break;
        
        make_eval_opcode_1(NOT, ~)
        make_eval_opcode_1(LNOT, !)
        make_eval_opcode_1(UADD, +)
        make_eval_opcode_1(USUB, -)
        
        make_eval_opcode_2(MUL, *)
        make_eval_opcode_2(DIV, /)
        make_eval_opcode_2(MOD, %)
        make_eval_opcode_2(ADD, +)
        make_eval_opcode_2(SUB, -)
        make_eval_opcode_2(LS, <<)
        make_eval_opcode_2(RS, >>)
        make_eval_opcode_2(L, <)
        make_eval_opcode_2(LE, <=)
        make_eval_opcode_2(G, >)
        make_eval_opcode_2(GE, >=)
        make_eval_opcode_2(EQU, ==)
        make_eval_opcode_2(NEQ, !=)
        make_eval_opcode_2(AND, &)
        make_eval_opcode_2(XOR, ^)
        make_eval_opcode_2(OR, |)
        make_eval_opcode_2(LAND, &&)
        make_eval_opcode_2(LOR, ||)
        
        make_eval_opcode_3(TRIOP);
        default:
            assert(0); // unregonized opcode
    };
}


/* check our hand-written version with flex-bison generated version 
   need to add additional compile options */
//#define WITH_CALC
//#define WITH_CALC_CHECK

#ifdef WITH_CALC
extern int calc_err_flag;
uint32_t dref(uint32_t x)
{
    return swaddr_read(x, 4);
}
uint32_t get_reg_val(char *name)
{
    int i;
    char buf[5];
    if (strlen(name) > 4) goto error;
    strncpy(buf, name, sizeof(buf));
    buf[sizeof(buf) - 1] = 0;
    
    if (buf[0] != '$') goto error;
    
    for (i = 1; i < strlen(buf); i++)
        buf[i] = tolower(buf[i]);
    
    if (strcmp(buf + 1, "eip") == 0)
        return cpu.eip;
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsl[i]) == 0)
            return reg_l(i);
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsw[i]) == 0)
            return reg_w(i);
    for (i = 0; i < 8; i++)
        if (strcmp(buf + 1, regsb[i]) == 0)
            return reg_b(i);

error:
    printf(c_red "  error: unknown register %s.\n" c_normal, name);
    calc_err_flag = 1;
    return 0;
}
#endif








static void add_opcode(int type, uint32_t *dest, uint32_t *op1, uint32_t *op2, uint32_t *op3)
{
    if (op2 == NULL) {
        if (type == ADD) type = UADD;
        else if (type == SUB) type = USUB;
        else if (type == MUL) type = DREF;
    }
    Opcode *cur = &opcodes[nr_opcode++];
    cur->type = type;
    cur->dest = dest;
    cur->op1 = op1;
    cur->op2 = op2;
    cur->op3 = op3;
}


static uint32_t *find_expr(int l, int r);

static uint32_t *find_operand_expr(int l, int r)
{
    if (l == r && tokens[l].type == NUM) return tokens[l].addr;
    if (r - l > 1 && tokens[l].type == LP && tokens[r].type == RP) return find_expr(l + 1, r - 1);
    return NULL;
}

static uint32_t *find_subscript_expr(int l, int r)
{
    uint32_t *ret;
    if ((ret = find_operand_expr(l, r))) return ret;
    if (tokens[r].type == SRP) {
        int i;
        for (i = l + 1; i < r; i++)
            if (tokens[i].type == SLP) {
                uint32_t *op1, *op2;
                op1 = find_subscript_expr(l, i - 1);
                if (op1) {
                    op2 = find_expr(i + 1, r - 1);
                    if (op2) {
                        uint32_t *ans = alloc_addr();
                        add_opcode(SUBSCRIPT, ans, op1, op2, NULL);
                        return ans;
                    }
                }
            }
    }
    return NULL;
}

#define make_unary_test_expr(TYPE) \
    if (tokens[l].type == TYPE) {                                   \
        uint32_t *op;                                               \
        op = find_unary_expr(l + 1, r);                             \
        if (op) {                                                   \
            uint32_t *ans = alloc_addr();                           \
            add_opcode(TYPE, ans, op, NULL, NULL);                  \
            return ans;                                             \
        }                                                           \
    }

static uint32_t *find_unary_expr(int l, int r)
{
    uint32_t *ret;
    if ((ret = find_subscript_expr(l, r))) return ret;
    if (l < r) {
        make_unary_test_expr(NOT)
        make_unary_test_expr(LNOT)
        make_unary_test_expr(ADD)
        make_unary_test_expr(SUB)
        make_unary_test_expr(MUL)
    }
    return NULL;
}

#define make_test_expr(TYPE)                                        \
    if (tokens[i].type == TYPE) {                                   \
        uint32_t *op1, *op2;                                        \
        op2 = next_expr(i + 1, r);                                  \
        if (op2) {                                                  \
            op1 = self_expr(l, i - 1);                              \
            if (op1) {                                              \
                uint32_t *ans = alloc_addr();                       \
                add_opcode(TYPE, ans, op1, op2, NULL);              \
                return ans;                                         \
            }                                                       \
        }                                                           \
    }

#define make_find_expr_helper(NAME, NEXT, LIST)                     \
    static uint32_t *find_ ## NAME ## _expr(int l, int r) {         \
        uint32_t *ret;                                              \
        uint32_t *(*self_expr)(int, int) = find_ ## NAME ## _expr;  \
        uint32_t *(*next_expr)(int, int) = find_ ## NEXT ## _expr;  \
        int i;                                                      \
        if ((ret = find_ ## NEXT ## _expr(l, r))) return ret;       \
        for (i = r - 1; i >= l + 1; i--) {                          \
            LIST                                                    \
        }                                                           \
        return NULL;                                                \
    }

make_find_expr_helper(MUL_DIV_MOD, unary, 
        make_test_expr(MUL)
        make_test_expr(DIV)
        make_test_expr(MOD)
    )

make_find_expr_helper(ADD_SUB, MUL_DIV_MOD, 
        make_test_expr(ADD)
        make_test_expr(SUB)
    )

make_find_expr_helper(LS_RS, ADD_SUB, 
        make_test_expr(LS)
        make_test_expr(RS)
    )

make_find_expr_helper(L_LE_G_GE, LS_RS, 
        make_test_expr(L)
        make_test_expr(LE)
        make_test_expr(G)
        make_test_expr(GE)
    )

make_find_expr_helper(EQU_NEQ, L_LE_G_GE, 
        make_test_expr(EQU)
        make_test_expr(NEQ)
    )

make_find_expr_helper(AND, EQU_NEQ, make_test_expr(AND))
make_find_expr_helper(XOR, AND, make_test_expr(XOR))
make_find_expr_helper(OR, XOR, make_test_expr(OR))
make_find_expr_helper(LAND, OR, make_test_expr(LAND))
make_find_expr_helper(LOR, LAND, make_test_expr(LOR))


static uint32_t *find_triop_expr(int l, int r)
{
    uint32_t *ret;
    if ((ret = find_LOR_expr(l, r))) return ret;
    int i, j;
    for (i = l + 1; i <= r - 3; i++)
        if (tokens[i].type == QMARK) {
            int cnt = 0;
            for (j = i + 1; j <= r - 1; j++) {
                if (tokens[j].type == QMARK) cnt++;
                if (tokens[j].type == CMARK) {
                    if (cnt == 0) {
                        uint32_t *op1, *op2, *op3;
                        op1 = find_LOR_expr(l, i - 1);
                        if (op1) {
                            op2 = find_triop_expr(i + 1, j - 1);
                            if (op2) {
                                op3 = find_triop_expr(j + 1, r);
                                if (op3) {
                                    uint32_t *ans = alloc_addr();
                                    add_opcode(TRIOP, ans, op1, op2, op3);
                                    return ans;
                                }
                            }
                        }
                        break;
                    } else {
                        cnt--;
                    }
                }
            }
        }
    return NULL;
}

static uint32_t *find_expr(int l, int r)
{
    uint32_t *ret;
    if ((ret = find_triop_expr(l, r))) return ret;
    return NULL;
}









struct expr_cache {
    char *expr;
    Opcode *opcodes;
    uint32_t *expr_mem;
    uint32_t *final_result;
    uint32_t last_result;
    int nr_opcode;
};

#define MAX_EXPR (1024 * 4)
static struct expr_cache expr_list[MAX_EXPR];
static int nr_expr = 0;
static uint32_t *last_result;
int expr_result_changed;
int quiet_eval = 0; // if quiet_eval == 1, then expr_result_changed will not be touched

const char *get_expr(int expr_id)
{
    return expr_list[expr_id].expr;
}

static void load_expr(int expr_id)
{
    opcodes = expr_list[expr_id].opcodes;
    //expr_mem = expr_list[expr_id].expr_mem; // optional
    final_result = expr_list[expr_id].final_result;
    nr_opcode = expr_list[expr_id].nr_opcode;
    last_result = &expr_list[expr_id].last_result;
    load_registers();
}

static void clean_expr()
{
    int i;
    for (i = 0; i < nr_expr; i++) {
        free(expr_list[nr_expr].expr);
        load_expr(i);
        clean_compile();
    }
    nr_expr = 0;
}

static int save_expr(char *e)
{
    assert(nr_expr < MAX_EXPR);
    expr_list[nr_expr].expr = strdup(e);
    expr_list[nr_expr].opcodes = opcodes;
    expr_list[nr_expr].expr_mem = expr_mem;
    expr_list[nr_expr].final_result = final_result;
    expr_list[nr_expr].nr_opcode = nr_opcode;
    return nr_expr++;
}

/* if found return expr-id, otherwise return -1 */
int find_expr_in_cache(char *e)
{
    int i;
    for (i = 0; i < nr_expr; i++)
        if (strcmp(expr_list[i].expr, e) == 0)
            return i;
    return -1;
}

/* compile a expr to expr cache
   if fail, returns -1
   otherwise, returns expr id
*/
int compile_expr(char *e)
{
    int expr_id;
    
    /* firstly, find expr in cache */
    expr_id = find_expr_in_cache(e);
    if (expr_id >= 0) return expr_id;
    
    
    //printf("do_compile: %s\n", e);
    /* do the real compile progress */
    assert(init_regex_flag == 1);
    prepare_compile();
	if(!make_token(e)) { // call lexer
	    clean_compile();
		return -1;
	}

    /*int i;//check lexer output
    for (i = 0; i < nr_token; i++)
        printf("t=%d addr=%p val=%d\n", tokens[i].type, tokens[i].addr, tokens[i].addr ? *tokens[i].addr : -1);*/
    	
    //printf("compile: %s\n", e);
    final_result = find_expr(0, nr_token - 1);
    if (!final_result) {
        clean_compile();
        return -1;
    }
    
    expr_result_changed = -1;
    return save_expr(e);
}

uint32_t eval_expr(int expr_id)
{
    int i;
    
    load_expr(expr_id);
    for (i = 0; i < nr_opcode; i++)
        eval_opcode(&opcodes[i]);
    if (!quiet_eval) {
        if (expr_result_changed == -1) expr_result_changed = 0;
        else expr_result_changed = *last_result != *final_result;
    }
    return *last_result = *final_result;
}


uint32_t expr(char *e, bool *success) {

#ifdef WITH_CALC_CHECK
    /* NOTE:
       since we use flex & bison to do this job,
       this function is just a wrapper of calc()
     */
    
    int flag;
    uint32_t calc(char *, int *);
    uint32_t ans = calc(e, &flag);
    //printf("expr=%s ans=%d flag=%d\n", e, ans, flag);
    //*success = flag == 1; return ans;
#endif
    
    
    /* NOTE:
       since we use expr() to implicate watchpoint feature
       the performance of expr() is very important
       the expr() has two steps:
           (1) call compile_expr() to compile a expr to opcodes
               since there are many string-operation & regex matches
               the compile progress is very slow
               but the compile step will only do once per expr
               since the opcodes will be saved for next time's use
           (2) call eval_expr() to eval opcodes()
               this step is much faster than the compile step
    */
    int expr_id = compile_expr(e);
    if (expr_id < 0) { /* expr compile failed */
        *success = false;
#ifdef WITH_CALC_CHECK
        assert(flag != 1);
#endif
        return 0;
    }

	uint32_t result = eval_expr(expr_id);
#ifdef WITH_CALC_CHECK
    assert(ans == result);
#endif
    *success = true;
    return result;
}



//#include <time.h>
/* expr test */
void expr_test()
{
#ifndef DEBUG
    return;
#endif
    bool success_flag;
    assert(expr("1+2+3+4", &success_flag) == 10); FORCE_USE(success_flag);
/*    int i; // preformance test
    clock_t t = clock();
    for (i = 0; i <= 1000000; i++) {
        expr("1+2", &success_flag);
    }
    printf("expr() time = %f\n", (double)(clock() - t) / CLOCKS_PER_SEC);*/
    clean_expr();
}
