#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>

/* find number meaning */
#define MEANING_LIMIT 3

#define m_test_func(name) concat(m_test_func_, name)
#define m_print_func(name) concat(m_print_func_, name)
#define m_test(name) int m_test_func(name) (uint32_t x)
#define m_print(name) void m_print_func(name) (uint32_t x)
#define m_table(name) { m_test_func(name), m_print_func(name) }

static m_test(ischar) { return x <= 0xFF && isprint(x); }
static m_print(ischar) { printf("CHAR '%c'", (char) x); }
static m_test(isdec) { return 1; }
static m_print(isdec) { printf("DEC '%d'", (int) x); }
static m_test(isbool) { return x == 0 || x == 1; }
static m_print(isbool) { printf("BOOL '%s'", x ? "TRUE" : "FALSE"); }

extern char *get_func_name(uint32_t loc, uint32_t *st, uint32_t *off);
static m_test(isfunc) { return get_func_name(x, NULL, NULL) != NULL; }
static m_print(isfunc)
{
    uint32_t off;
    char *fname = get_func_name(x, NULL, &off);
    printf("CODE " c_purple "'%s+%04X'" c_normal, fname, off);
}

/* minmal string length to print */
#define STRING_MIN_PRINT 1
/* maximum string length to print */
#define STRING_MAX_PRINT 20
/* maximum string length to check */
#define STRING_MAX_CHECK 100

static m_test(isstring)
{
    /* first: check if x is a string */
    int i;
    for (i = 0; i < STRING_MAX_CHECK; i++) {
        int ch, success;
        ch = safe_swaddr_read(x + i, 1, &success, R_DS);
        if (!success) return 0;
        if (ch == 0) break;
        if (!isprint(ch)) return 0;
    }
    return i >= STRING_MIN_PRINT;
}

static m_print(isstring)
{
    printf("STR \"");
    int i;
    for (i = 0; i < STRING_MAX_PRINT; i++) {
        int ch;
        ch = safe_swaddr_read(x + i, 1, NULL, R_DS);
        if (ch == 0) break;
        putchar(i < STRING_MAX_PRINT - 3 ? ch : '.');
    }
    printf("\"");
}

void print_number_meaning(uint32_t x)
{
    static struct {
        int (*test)(uint32_t);
        void (*print)(uint32_t);
    } func[] = {
        m_table(isfunc),
        m_table(isdec),
        m_table(ischar),
        m_table(isbool),
        m_table(isstring),
    };
    
    int cnt = 0;
    int i;
    for (i = 0; i < (sizeof(func) / sizeof(func[0])); i++)
        if (func[i].test(x)) {
            cnt++;
            if (cnt != 1) printf(", ");
            func[i].print(x);
            
            if (cnt >= MEANING_LIMIT)
                break;
        }
    
    if (!cnt) printf("?");
}












void print_registers()
{
    uint32_t new_val, old_val;
    char new_str[9], old_str[9];
    int changed_flag;
    int i;
    int p;
    for (i = 0; i < 8; i++) {
        old_val = old_cpu.gpr[i]._32;
        new_val = cpu.gpr[i]._32;
        changed_flag = (old_cpu.eip != 0 && old_val != new_val);
        
        printf("  ");
        if (changed_flag) printf(c_red c_bold "%s" c_normal c_yellow ":" c_normal, regsL[i]);
        else printf(c_yellow "%s:" c_normal, regsL[i]);
        
        printf(" ");
        
        sprintf(old_str, "%08X", old_val);
        sprintf(new_str, "%08X", new_val);
        for (p = 0; p < 8; p++)
            if (old_cpu.eip != 0 && old_str[p] != new_str[p])
                printf(c_red c_bold "%c" c_normal, new_str[p]);
            else
                printf("%c", new_str[p]);
        
        printf("  ("); print_number_meaning(new_val); printf(")");
        
        printf("\n");
    }
    
    printf("\n  " c_yellow "EIP:" c_normal " %08X  ", cpu.eip);
    uint32_t cur_off;
    char *cur_func_name = get_func_name(cpu.EIP, NULL, &cur_off);
    if (!cur_func_name) { cur_func_name = "?"; cur_off = 0; }
    printf(c_purple "(%s+%04X)\n" c_normal, cur_func_name, cur_off);
    
    printf("  " c_yellow "EFLAGS: ");
    printf(c_normal "%08X ", cpu.EFLAGS.EFLAGS);
#ifdef INVF_CHECK
#define PRINT_FLAG(F) printf("%s", cpu.EFLAGS_INVALID.F ? c_bold c_blue : (cpu.EFLAGS.F ? c_bold c_red : c_normal)); \
    printf("%c" #F " ", cpu.EFLAGS_INVALID.F ? (cpu.EFLAGS.F ? 'X' : 'x') : (cpu.EFLAGS.F ? '+' : '-'));
#else
#define PRINT_FLAG(F) printf("%s", (cpu.EFLAGS.F ? c_bold c_red : c_normal)); \
    printf("%c" #F " ", (cpu.EFLAGS.F ? '+' : '-'));
#endif
    PRINT_FLAG(CF)
    PRINT_FLAG(PF)
    PRINT_FLAG(ZF)
    PRINT_FLAG(SF)
    PRINT_FLAG(IF)
    PRINT_FLAG(DF)
    PRINT_FLAG(OF)
    printf(c_normal "\n");
    
#define PRINT_OTHER_FLAG(F) printf("%s", (cpu.F ? c_bold c_red : c_normal)); \
    printf("%c" #F " ", (cpu.F ? '+' : '-'));
    
    printf("  " c_yellow "CR0: " c_normal "%08X ", cpu.CR0);
    PRINT_OTHER_FLAG(PG)
    PRINT_OTHER_FLAG(ET)
    PRINT_OTHER_FLAG(TS)
    PRINT_OTHER_FLAG(EM)
    PRINT_OTHER_FLAG(MP)
    PRINT_OTHER_FLAG(PE)
    printf(c_normal "\n");
    
    printf("  " c_yellow "CR3: " c_normal "%08X\n", cpu.CR3);
    printf("  " c_yellow "GDTR: " c_normal "%08X L:%04X\n", cpu.GDTR, cpu.GDT_LIMIT);
    printf("  " c_yellow "IDTR: " c_normal "%08X L:%04X\n", cpu.IDTR, cpu.IDT_LIMIT);
    
    printf("  " c_yellow "ES: " c_normal "%04X B:%08X L:%08X\n", cpu.ES, cpu.seg_base[R_ES], cpu.seg_limit[R_ES]);
    printf("  " c_yellow "CS: " c_normal "%04X B:%08X L:%08X\n", cpu.CS, cpu.seg_base[R_CS], cpu.seg_limit[R_CS]);
    printf("  " c_yellow "SS: " c_normal "%04X B:%08X L:%08X\n", cpu.SS, cpu.seg_base[R_SS], cpu.seg_limit[R_SS]);
    printf("  " c_yellow "DS: " c_normal "%04X B:%08X L:%08X\n", cpu.DS, cpu.seg_base[R_DS], cpu.seg_limit[R_DS]);
    printf("  " c_yellow "FS: " c_normal "%04X B:%08X L:%08X\n", cpu.FS, cpu.seg_base[R_FS], cpu.seg_limit[R_FS]);
    printf("  " c_yellow "GS: " c_normal "%04X B:%08X L:%08X\n", cpu.GS, cpu.seg_base[R_GS], cpu.seg_limit[R_GS]);

}




struct dm_history {
    uint32_t addr;
    int n;
    char *mem;
    int cnt;
};
#define MAX_DM_HISTORY 1024
void dump_memory(uint32_t addr, int n)
{
    if (n == 0) return;
    static struct dm_history hist[MAX_DM_HISTORY];
    char *last_dump;
    int new_flag = 0;
    int mcnt = INT_MAX, r = 0;
    int i, j;
    
    /* find histroy slot */
    for (i = 0; i < MAX_DM_HISTORY; i++)
        if (hist[i].addr == addr && hist[i].n == n) {
            last_dump = hist[i].mem;
            hist[i].cnt++;
            break;
        } else if (hist[i].addr == 0 && hist[i].n == 0) {
            hist[i].addr = addr;
            hist[i].n = n;
            last_dump = hist[i].mem = malloc(n);
            hist[i].cnt = 1;
            new_flag = 1;
            break;
        } else if (hist[i].cnt < mcnt) {
            r = i;
            mcnt = hist[i].cnt;
        }
    if (i == MAX_DM_HISTORY) {
        printf(c_yellow "  WARNING: no more history slot, replacing %d." c_normal "\n", r);
        hist[r].addr = addr;
        hist[r].n = n;
        free(hist[r].mem);
        last_dump = hist[r].mem = malloc(n);
        hist[r].cnt = 1;
        new_flag = 1;
    }
    
    
    
    printf(c_green "  %d bytes of memory dump at " c_blue "0x%08X" c_normal "\n", n, addr);
    
    printf(c_yellow
           "            +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F" c_normal "\n");
    //     "  AABBCCDD  aa bb cc dd 00 11 22 33  dd cc bb aa 33 22 11 00  ................");


    /* THE CODE BELOW IS VERY UGLY, BUT IT WORKS!!!
       NOBODY (INCLUDING ZBY) CAN UNDERSTAND IT AFTER FEW DAYS!
       ZBY IS TOO LAZY TO REWRITE IT
    */
    char buf[16];
    char b;
    int m = n % 16 ? n - n % 16 + 16 : n;
    int changed_flag = 0;
    char lb[16];
    int bcflag[16] = {};
    for (i = 0; i < m; i++) {
        if (i % 16 == 0) {
            for (j = 0; j < 16; j++)
                if (i + j < n)
                    lb[j] = safe_swaddr_read(addr + i + j, 1, NULL, R_DS);
                else
                    lb[j] = 0xcc;
        
            changed_flag = 0;
            for (j = 0; j < 16; j++)
                if (!new_flag && i + j < n && lb[j] != last_dump[i + j])
                    bcflag[j] = changed_flag = 1;
                else
                    bcflag[j] = 0;
            if (changed_flag)
                printf(c_red c_bold "  %08x  " c_normal, addr + i);
            else
                printf(c_yellow "  %08x  " c_normal, addr + i);
        }
        
        if (i < n) {
            b = lb[i % 16];
            if (!bcflag[i % 16])
                printf("%02x ", b & 0xff);
            else
                printf(c_red c_bold "%02x " c_normal, b & 0xff);
            last_dump[i] = b;
        } else {
            b = '.';
            printf(".. ");
        }
        buf[i % 16] = isprint(b) ? b : '.';
        
        if (i % 16 == 7)
            printf(" ");
        
        if (i % 16 == 15) {
            printf(" ");
            for (j = 0; j < 16; j++)
                if (!bcflag[j])
                    printf("%c", buf[j]);
                else
                    printf(c_red c_bold "%c" c_normal, buf[j]);
            printf("\n");
        }
    }
}







#define is_prefix(a, b) (strlen((a)) <= strlen((b)) && strncmp((a), (b), strlen((a))) == 0)

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;
    static char *line_copy = NULL;
    
    line_read = readline("(nemu) ");
    if (!line_read) {
        // if Ctrl-D pressed (EOF reached), this pointer is NULL
        free(line_read);
        line_read = strdup("q");
        puts("q");
    }

	if (*line_read) { // string is not null, use new line
	    free(line_copy);
	    line_copy = strdup(line_read);
		add_history(line_read);
	} else if (line_copy) { // string is null, use last line
	    free(line_read);
	    line_read = strdup(line_copy);
	}

	return line_read;
}



/*  stack frame like this linked-list
    typedef struct {
        swaddr_t prev_ebp;
        swaddr_t ret_addr;
        uint32_t args[4];
    } PartOfStackFrame;
*/
extern char *get_func_name(uint32_t loc, uint32_t *st, uint32_t *off);
#define MAX_ARGS_PRINT 4
#define MAX_FRAME_PRINT 20
static int cmd_bt(char *args)
{
    int i;
    int success;
    int frame_cnt = 0;
    
    int n = MAX_FRAME_PRINT;
    if (args) {
        bool success;
        n = expr(args, &success);
        if (!success) {
            puts(c_red "  unrecognized number" c_normal);
            return 1;
        }
    }
    
    uint32_t cur_frame = cpu.EBP;
    uint32_t cur_off;
    char *cur_func_name = get_func_name(cpu.EIP, NULL, &cur_off);
    if (!cur_func_name) { cur_func_name = "?"; cur_off = 0; }
        printf(c_purple "  current frame: %s+%04X (%08X)\n" c_normal, cur_func_name, cur_off, cpu.EIP);
    
    while (1) {
        uint32_t func_args[MAX_ARGS_PRINT];
        for (i = 0; i < MAX_ARGS_PRINT; i++) {
            func_args[i] = safe_swaddr_read(cur_frame + 8 + i * 4, 4, &success, R_SS);
            if (!success) goto read_fail;
        }
        for (i = 0; i < MAX_ARGS_PRINT; i++) {
            printf("   " c_yellow "arg[%d]: " c_normal, i);
            printf("%08X", func_args[i]);
            printf("  ("); print_number_meaning(func_args[i]); printf(")\n");
        }
        
        uint32_t next_frame = safe_swaddr_read(cur_frame, 4, &success, R_SS);
        if (!success) goto read_fail;
        if (next_frame == 0) return 0;
        uint32_t ret_addr = safe_swaddr_read(cur_frame + 4, 4, &success, R_SS);
        if (!success) goto read_fail;
        
        uint32_t off;
        char *func_name = get_func_name(ret_addr, NULL, &off);
        if (!func_name) { func_name = "?"; off = 0; }
        printf(c_red "  return to: %s+%04X (%08X)\n" c_normal, func_name, off, ret_addr);
        
        cur_frame = next_frame;
        if (++frame_cnt > n) {
            puts(c_red "  error: backtrack limit exceed\n" c_normal);
            return 1;
        }
    }
read_fail:
    puts(c_red "  error: swaddr_read failed, can't continue\n" c_normal);
    return 1;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

extern int expr_result_changed;
static int cmd_p(char *args) {
    if (!args) {
        puts(c_red "  No expression specified." c_normal);
        return 1;
    }
    
    char *saveptr;
    int cnt = 0;
    while (1) {
        char *e = strtok_r(args, ",", &saveptr);
        args = NULL;
        if (!e) break;
        bool success;
        uint32_t ans = expr(e, &success);
        if (!success) {
            puts(c_red "  Error occured during calculation." c_normal);
            return 1;
        }
        printf(c_yellow "  ans[%d]: ", cnt++);
        
        if (strlen(e) > 10) printf(c_blue "%.7s...", e); else printf(c_blue "%-10s", e);
        
        printf(c_yellow " = " c_normal);
        if (expr_result_changed) printf(c_bold c_red);
        printf("0x%08X", ans);
        if (expr_result_changed) printf(c_normal);
        //printf(" = %u", ans);
        printf("  ("); print_number_meaning(ans); printf(")");
        printf("\n");
    }
	return 0;
}

static int cmd_help(char *args);

static int cmd_si(char *args)
{
    int n = 1;
    if (args) {
        if (sscanf(args, "%d", &n) != 1) {
            puts(c_red "  unrecognized number" c_normal);
            return 1;
        }
    }
    cpu_exec(n);
    return 0;
}

static int cmd_ni(char *args)
{
    cpu_exec(0);
    return 0;
}

static int cmd_zx(char *args)
{
    char *p;
    int n;
    bool success;
    
    if (!args || (p = strchr(args, ' ')) == NULL) {
        puts(c_red "  zx N EXPR: dump N bytes of memory at EXPR" c_normal);
        return 1;
    }
    
    *p = '\0';
    n = expr(args, &success);
    if (!success) {
        puts(c_red "  unrecognized number" c_normal);
        return 1;
    }
    
    char *e = p + 1;
    uint32_t m = expr(e, &success); if (!success) return 1;
    
    dump_memory(m, n);
    return 0;
}

static int cmd_info(char *args)
{
    if (!args || (*args != 'r' && *args != 'w' && *args != 's' && *args != 'c' && *args != 't')) {
        puts(c_red "  info SUBCMD: no subcmd specified (register or watchpoint or symbol or cache or tlb)" c_normal);
        return 1;
    }
    
    if (*args == 'r') {
        // show register information
        print_registers();
    } else if (*args == 'w') {
        // show watchpoint information
        extern void info_wp();
        info_wp();
    } else if (*args == 's') {
        extern void show_elf_symbols();
        show_elf_symbols();
    } else if (*args == 'c') {
        #ifdef USE_MEMORY_CACHE
        extern void l1cache_show_perfcounter();
        printf(c_red " L1:\n" c_normal);
        l1cache_show_perfcounter();
        printf("\n");
        extern void l2cache_show_perfcounter();
        printf(c_red " L2:\n" c_normal);
        l2cache_show_perfcounter();
        printf("\n");
        #else
        printf("  %scache disabled%s\n", c_red, c_normal);
        #endif
    } else if (*args == 't') {
        extern void show_tlb();
        printf(c_red " TLB:\n" c_normal);
        show_tlb();
        printf("\n");
    }
    return 0;
}

extern int add_wp(char *, int);
static int cmd_w(char *args)
{
    if (!args) {
        puts(c_red "  watch EXPR: stop execute if value of EXPR changes" c_normal);
        return 1;
    }
    
    int wp_id = add_wp(args, 0);
    if (wp_id < 0) {
        puts(c_red "  expr compile failed." c_normal);
        return 1;
    }
    
    return 0;
}

static int cmd_b(char *args)
{
    /* very like cmd_w */
    if (!args) {
        puts(c_red "  b EXPR: stop execute if EIP == EXPR" c_normal);
        return 1;
    }
    
    char *buf = malloc(strlen(args) + 10);
    sprintf(buf, "(%s)==$eip", args);
    int wp_id = add_wp(buf, 1);
    free(buf);
    
    if (wp_id < 0) {
        puts(c_red "  expr compile failed." c_normal);
        return 1;
    }
    
    return 0;
}

extern int remove_wp(int wp_id);
static int cmd_d(char *args)
{
    int wp_id;
    if (!args || sscanf(args, "%d", &wp_id) != 1) {
        puts(c_red "  delete ID: delete watchpoint ID" c_normal);
        return 1;
    }
    
    if (!remove_wp(wp_id)) {
        puts(c_red "  no such watchpoint" c_normal);
        return 1;
    }
    
    printf(c_blue "  watchpoint #%d deleted." c_normal "\n", wp_id);
    return 0;
}



static int cmd_z(char *args)
{
    puts(c_red "  use zX to call zby's own commands. (X is command name)" c_normal);
    return 0;
}


struct sm_history {
    uint32_t addr;
    int n;
    uint32_t *mem;
    int cnt;
};

#define MAX_SM_HISTORY 1024

void scan_memory(uint32_t addr, int n, int show_ebp_offset)
{
    if (n == 0) return;
    
    static struct sm_history hist[MAX_SM_HISTORY];
    int i;
    int new_flag = 0;
    int mcnt = INT_MAX;
    int r = 0;
    uint32_t *val;
    /* find histroy slot */
    for (i = 0; i < MAX_SM_HISTORY; i++)
        if (hist[i].addr == addr && hist[i].n == n) {
            val = hist[i].mem;
            hist[i].cnt++;
            break;
        } else if (hist[i].addr == 0 && hist[i].n == 0) {
            hist[i].addr = addr;
            hist[i].n = n;
            val = hist[i].mem = malloc(n * 4);
            hist[i].cnt = 1;
            new_flag = 1;
            break;
        } else if (hist[i].cnt < mcnt) {
            r = i;
            mcnt = hist[i].cnt;
        }
    if (i == MAX_SM_HISTORY) {
        printf(c_yellow "  WARNING: no more history slot, replacing %d." c_normal "\n", r);
        hist[r].addr = addr;
        hist[r].n = n;
        free(hist[r].mem);
        val = hist[r].mem = malloc(n * 4);
        hist[r].cnt = 1;
        new_flag = 1;
    }

    for (i = 0; i < n; i++) {
        uint32_t old_val = val[i];
        val[i] = safe_swaddr_read(addr + i * 4, 4, NULL, R_DS);
        printf(c_yellow "  [0x%08X] = " c_normal, addr + i * 4);
        if (show_ebp_offset) {
            int off = (addr + i * 4) - cpu.EBP;
            
            if (0x8 <= off && off <= 0x4 + 4 * 4) {
                printf(c_purple "[ARG %d   ] = " c_normal, (off - 0x8) / 4 + 1);
            } else if (off == 0) {
                printf(c_green "[EBP     ] = " c_normal);
            } else if (off == 0x4) {
                printf(c_green "[RETNADDR] = " c_normal);
            } else {
                printf(c_blue "[EBP%c%04X] = " c_normal, off >= 0 ? '+' : '-', off >= 0 ? off : -off);
            }
        }
        int changed = (!new_flag && old_val != val[i]);
        
        if (changed) printf(c_bold c_red);
        printf("%08X", val[i]);
        if (changed) printf(c_normal);
        
        printf(" (");
        print_number_meaning(val[i]);
        printf(")\n");
    }
}

static int cmd_x(char *args)
{
    char *p;
    int n;
    bool success;
    
    if (!args || (p = strchr(args, ' ')) == NULL) {
        puts(c_red "  x N EXPR: scan 4*N bytes of memory at EXPR" c_normal);
        return 1;
    }
    
    *p = '\0';
    n = expr(args, &success);
    if (!success) {
        puts(c_red "  unrecognized number" c_normal);
        return 1;
    }
    
    char *e = p + 1;
    uint32_t m = expr(e, &success); if (!success) return 1;
    
    scan_memory(m, n, 0);
    return 0;
}

#define MAX_STACKDUMP_LIMIT 100
static int cmd_zs(char *args)
{
    /* equal to this command:
       x (0x8000000-$esp)/4 $esp
    */
    
    int n = (0x8000000 - cpu.ESP) / 4 - 4;
    if (args) {
        bool success;
        n = expr(args, &success);
        if (!success) {
            puts(c_red "  unrecognized number" c_normal);
            return 1;
        }
    }
    if (n > MAX_STACKDUMP_LIMIT) {
        printf(c_red "  error: stack too large or invalid\n" c_normal);
        return 1;
    }
    scan_memory(cpu.ESP, n, 1);
    return 0;
}


extern char *exec_file;
#define MAX_TXT_LINE 1048576
char *txt_line[MAX_TXT_LINE];
int txt_line_cnt = -1;

static void load_txt_file()
{
    char fn[128];
    char buf[1024];
    snprintf(fn, sizeof(fn), "%s.txt", exec_file);
    FILE *f = fopen(fn, "r");
    if (!f) {
        printf(c_red "  error: can't open file %s\n" c_normal, fn);
        txt_line_cnt = 0;
        return;
    }
    int l = 0;
    while (fgets(buf, sizeof(buf), f)) {
        if (l >= MAX_TXT_LINE) {
            printf(c_red "  error: no more line buffer for file %s\n" c_normal, fn);
            break;
        }
        txt_line[l] = strdup(buf);
        l++;
    }
    txt_line_cnt = l;
    fclose(f);
}

int show_as_code(int base, int range)
{
    if (txt_line_cnt < 0) load_txt_file();
    int i;
    for (i = 0; i < txt_line_cnt; i++) {
        char *buf = txt_line[i];
        unsigned addr;
        if (sscanf(buf, "%x", &addr) == 1) {
            if ((base == 0 && cpu.EIP - range <= addr && addr <= cpu.EIP + range) ||
                (base != 0 && base <= addr && addr <= base + range)) {
                if (addr != cpu.EIP || range == 0)
                    printf("%s", buf);
                else {
                    if (*buf == ' ') buf++;
                    if (*buf == ' ') buf++;
                    printf("=>%s", buf);
                }
            }
        }
    }
    return i >= txt_line_cnt ? 0 : 1;
}

static int cmd_list(char *args)
{
    char *p;
    int n = 0;
    bool success;
    uint32_t m = 0x20;
    
    if (args) {
        p = strchr(args, ' ');
        
        if (p) *p = '\0';
        n = expr(args, &success);
        if (!success) {
            puts(c_red "  list N R: list program at [N, N+R] (or [EIP-R, EIP+R] if N == 0)" c_normal);
            puts(c_red "  unrecognized number" c_normal);    
            return 1;
        }
        
        if (p) {
            char *e = p + 1;
            m = expr(e, &success); if (!success) return 1;
        }
    }
    
    show_as_code(n, m);
    return 0;
}

static int cmd_cache(char *args) {
    #ifdef USE_MEMORY_CACHE
    if (!args) {
        puts(c_red "  No addr specified." c_normal);
        return 1;
    }
    
    bool success;
    uint32_t ans = expr(args, &success);
    if (!success) {
        puts(c_red "  Error occured during calculation." c_normal);
        return 1;
    } else {
        printf(c_red " L1:\n" c_normal);
        extern void l1cache_show_cacheline(unsigned);
        l1cache_show_cacheline(ans);
        
        printf(c_red " L2:\n" c_normal);
        extern void l1cache_show_cacheline(unsigned);
        l1cache_show_cacheline(ans);
    }
    #else
    printf("  %scache disabled%s\n", c_red, c_normal);
    #endif
	return 0;
}


static int cmd_page(char *args) {
    if (!args) {
        puts(c_red "  No addr specified." c_normal);
        return 1;
    }
    
    bool success;
    uint32_t ans = expr(args, &success);
    if (!success) {
        puts(c_red "  Error occured during calculation." c_normal);
        return 1;
    } else {
        printf("  linear addr: %08X\n", ans);
        
        extern char page_errbuf[];
        extern int page_nopanic;
        extern int page_errflag;
        
        page_nopanic = 1;
        page_errflag = 0;
        
        extern hwaddr_t page_translate_nocache(lnaddr_t addr);
        hwaddr_t pa = page_translate_nocache(ans);
        if (page_errflag) {
            printf("%s", c_red);
            printf("  page-translate error!\n");
            printf("    %s\n", page_errbuf);
            printf("%s", c_normal);
        } else {
            printf("  physical addr: %08X\n", pa);
        }
        
        page_nopanic = 0;
        page_errflag = 0;
    }
	return 0;
}

static int cmd_perf(char *args) {
    if (args && *args == 'r') {
        printf("  reset pref data\n");
        void reset_record();
        reset_record();
        return 0;
    }
    printf("  perf report:\n");
    extern void show_perf_data();
    show_perf_data();
    return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {

/* DONE: Add more commands */
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "continue", "Continue the execution of the program", cmd_c },
	{ "run", "Same as continue", cmd_c },
	
    { "si", "Run single instruction", cmd_si },
    { "ni", "Run program until next instruction", cmd_ni },
    { "info", "Show information of [r]egister or [w]atchpoint or [s]ymbol or [c]ache or [t]lb", cmd_info },
    { "print", "Print value of an expression (',' to split multiple expressions)", cmd_p },
    { "x", "Dump memory", cmd_x },
    { "watch", "Set a watchpoint", cmd_w },
    { "delete", "Delete a watchpoint", cmd_d },
    { "b", "Set a breakpoint", cmd_b },
    { "bt", "Backtrace of all stack frames", cmd_bt },
    { "list", "List assemble code in txt file", cmd_list },
    
    { "cache", "Show cache line by addr", cmd_cache },
    { "page", "Show page translation result", cmd_page },

    { "perf", "Show perf report", cmd_perf },
    
	{ "z", "Zby's own commands (use zX to call, X is command name)", cmd_z },
	{ "zx", "Memory dump", cmd_zx },
	{ "zs", "Stack dump", cmd_zs },
	
	{ "quit", "Exit NEMU", cmd_q },
//	{ "exit", "Exit NEMU", cmd_q },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL) {
		/* no argument given */
		
		printf("\n");
		
		// print hello message
		printf(c_purple);
        printf("  Welcome to NEMU, a ICS project for Fudan University.\n");
        printf("    Original Project: http://fdu-ics.gitbooks.io/programming-assignment\n");
        printf("    Improved by: Zhang Boyang (http://www.zbyzbyzby.com)\n");
        printf(c_normal "\n");
        
        // print command list
        printf(c_blue);
        printf("  Command List:\n");
		for(i = 0; i < NR_CMD; i ++) {
			printf("    %s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
		printf(c_normal "\n");
		
		// print hints
		printf(c_green);
        printf("  Hints:\n");
		printf("    you can use ';' to execute more commands in single line.\n");
		printf("    you can use command prefix instead of full command.\n");
		printf(c_normal "\n");
	} else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("  %s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("  Unknown command '%s'\n", arg);
	}
	return 0;
}

extern int ui_safe_read_failed;
void ui_mainloop() {
	while(1) {
	    char *input = rl_gets();
	    char *saveptr = NULL;
	    while (1) {
		    char *str = strtok_r(input, ";", &saveptr);
		    input = NULL;
		    if (!str) break;
		    while (*str == ' ') str++;
		    if (!*str) continue;
		    
		    char *str_end = str + strlen(str);

		    /* extract the first token as the command */
		    char *cmd = strtok(str, " ");
		    if (cmd == NULL) { continue; }

		    /* treat the remaining string as the arguments,
		     * which may need further parsing
		     */
		    char *args = cmd + strlen(cmd) + 1;
		    if(args >= str_end) {
			    args = NULL;
		    }

    #ifdef HAS_DEVICE
		    extern void sdl_clear_event_queue(void);
		    sdl_clear_event_queue();
    #endif

#ifndef REG_DIFF_HIGHLIGHT
                /* last cpustate disabled, so copy current cpu_state to old_cpu */
                old_cpu = cpu;
#endif
            
		    int i;
		    for(i = 0; i < NR_CMD; i ++) {
			    if (strcmp(cmd, cmd_table[i].name) == 0) {
			        printf(c_yellow "command: %s %s" c_normal "\n", cmd_table[i].name, args ? args : "");
				    if (cmd_table[i].handler(args) < 0) { return; }
				    break;
			    }
		    }
		    if (i < NR_CMD) goto done;
		
		    for(i = 0; i < NR_CMD; i ++) {
			    if (is_prefix(cmd, cmd_table[i].name)) {
			        printf(c_yellow "command: " c_green "%s" c_yellow " %s" c_normal "\n", cmd_table[i].name, args ? args : "");
			        if (cmd_table[i].handler(args) < 0) { return; }
			        break;
			    }
		    }
		    if (i < NR_CMD) goto done;
		
		    printf(c_red "  Unknown command '%s'" c_normal "\n", cmd);
		    continue;

done:
            if (ui_safe_read_failed) {
                printf(c_red "  WARNING: error occured during swaddr_read" c_normal "\n");
                ui_safe_read_failed = 0;
            }
		}
	}
}
