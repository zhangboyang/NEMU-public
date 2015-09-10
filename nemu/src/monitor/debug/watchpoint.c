#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 128

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

/* DONE: Implement the functionality of watchpoint */

const char *get_expr(int expr_id);
uint32_t eval_expr(int expr_id);
int compile_expr(char *e);
void print_number_meaning(uint32_t x);
extern int expr_result_changed;
extern int quiet_eval;

#define MAX_WP_EXPR_PRINT 15
static void print_wp(WP *cur)
{
    quiet_eval = 1;
    uint32_t val = eval_expr(cur->expr_id);
    quiet_eval = 0;
    const char *e = get_expr(cur->expr_id);
    printf(c_yellow "   wp[%d]: ", cur->NO);
    int l = MAX_WP_EXPR_PRINT;
    if (strlen(e) > l) printf(c_blue "%.*s...", l - 3, e); else printf(c_blue "%*s", -l, e);
    printf(c_yellow " = ");
    if (cur->hit)
        printf(c_red c_bold "0x%08x" c_normal, val);
    else
        printf(c_normal "0x%08x", val);
    printf("  (");
    print_number_meaning(val);
    printf(")\n");
}

int remove_wp(int wp_id)
{
    WP *cur;
    if (head && head->NO == wp_id) {
        WP *d = head;
        head = d->next;
        d->next = free_;
        free_ = d;
        return 1;
    }
    for (cur = head; cur != NULL; cur = cur->next)
        if (cur->next && cur->next->NO == wp_id) {
            WP *d = cur->next;
            cur->next = d->next;
            d->next = free_;
            free_ = d;
            return 1;
        }
    return 0;
}

void info_wp()
{
    WP *cur;
    if (head == NULL) puts(c_yellow "  no watchpoints" c_normal);
    for (cur = head; cur != NULL; cur = cur->next)
        print_wp(cur);
}

int add_wp(char *e, int type)
{
    WP *cur;
    for (cur = head; cur != NULL; cur = cur->next)
        if (strcmp(e, get_expr(cur->expr_id)) == 0)
            goto done;

    int expr_id = compile_expr(e);
    if (expr_id < 0) return -1;
    eval_expr(expr_id);
    
    cur = free_;
    assert(cur);
    free_ = free_->next;
    cur->next = head;
    head = cur;

    cur->expr_id = expr_id;
    cur->hit = 0;
    cur->type = type;

done:
    print_wp(cur);
    return cur->NO;
}


int eval_wp()
{
    int flag = 0;
    WP *cur;
    for (cur = head; cur != NULL; cur = cur->next) {
        uint32_t val = eval_expr(cur->expr_id);
        if ((!cur->type && expr_result_changed) || (cur->type && val)) {
            cur->hit = 1;
            flag = 1;
        } else {
            cur->hit = 0;
        }
    }
    return flag;
}
