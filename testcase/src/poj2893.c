char input_buffer[] = 
"\x33\x20\x33\x0A\x31\x20\x30\x20\x33\x0A"
"\x34\x20\x32\x20\x35\x0A\x37\x20\x38\x20"
"\x36\x0A\x34\x20\x33\x0A\x31\x20\x32\x20"
"\x35\x0A\x34\x20\x36\x20\x39\x0A\x31\x31"
"\x20\x38\x20\x31\x30\x0A\x33\x20\x37\x20"
"\x30\x0A\x33\x20\x34\x0A\x31\x20\x34\x20"
"\x31\x31\x20\x33\x0A\x32\x20\x36\x20\x38"
"\x20\x37\x0A\x35\x20\x39\x20\x31\x30\x20"
"\x30\x0A\x30\x20\x30\x0A"
;

char answer_buffer[] = 
"\x59\x45\x53\x0A\x4E\x4F\x0A\x4E\x4F\x0A"
""
;

#include "trap.h"
#include <stdarg.h>
#include <string.h>

/* TEMPLATE CODE */

char output_buffer[sizeof(answer_buffer) + 0x100];


/* only usable when a >= 0 */
void LLdiv10(unsigned long long a, unsigned long long *q, int *r)
{
    union {
        long long value;
        unsigned short s[4];
    } LL;
    
    int i;
    unsigned int cur, sq, sr = 0;
    LL.value = a;

    for (i = 3; i >= 0; i--) {
        cur = sr * 0x10000 + LL.s[i];
        sq = cur / 10;
        sr = cur % 10;
        LL.s[i] = sq;
    }

    *q = LL.value;
    *r = sr;
}

/* useful functions */

int naive_isdigit(char c)
{
    return '0' <= c && c <= '9';
}

int naive_isspace(char c)
{
    switch (c) {
        case ' ': case '\f': case '\n': case '\r': case '\t': case '\v':
            return 1;
        default:
            return 0;
    }
}

/* write_TYPE(): print TYPE to output buffer
 * return outputed char cnt
 */

char *write_ptr = output_buffer;

void finish_output()
{
    *write_ptr = '\0';
}

int write_char(char c)
{
    *write_ptr++ = c;
    return 1;
}

int write_string(char *s)
{
    int cnt = 0;
    while (*s) cnt += write_char(*s++);
    return cnt;
}

int write_llint(long long lld)
{
    int buf[100];
    int p = 0, ret;
    unsigned long long llu;
    if (lld < 0) { write_char('-'); lld = -lld; }
    llu = lld;
    do {
        LLdiv10(llu, &llu, &buf[p++]);
    } while (llu > 0);
    ret = p;
    while (p > 0) write_char('0' + buf[--p]);
    return ret;
}

int write_int(int d)
{
    char buf[100];
    int p = 0, ret;
    unsigned int u;
    if (d < 0) { write_char('-'); d = -d; }
    u = d;
    do {
        buf[p++] = u % 10;
        u /= 10;
    } while (u > 0);
    ret = p;
    while (p > 0) write_char('0' + buf[--p]);
    return ret;
}

int naive_printf(const char *fmt, ...)
{
    va_list ap;
    long long lld;
    int d;
    char c, *s;
    int cnt = 0;
    
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    s = va_arg(ap, char *);
                    cnt += write_string(s);
                    break;
                case 'd':
                    d = va_arg(ap, int);
                    cnt += write_int(d);
                    break;
                case 'c':
                    c = va_arg(ap, int);
                    cnt += write_char(c);
                    break;
                case 'l':
                    if (*++fmt == 'l' && *++fmt == 'd') {
                        lld = va_arg(ap, long long);
                        cnt += write_llint(lld);
                    }
                    break;
                case '%':
                    cnt += write_char('%');
                    break;
            }
        } else {
            cnt += write_char(*fmt);
        }
        fmt++;
    }
    va_end(ap);
    return cnt;
}


/* read_TYPE(): read TYPE from input buffer
 * return read object count
 */

char *read_ptr = input_buffer;

int read_char(char *cp)
{
    if (*read_ptr) {
        *cp = *read_ptr++;
        return 1;
    } else {
        return 0;
    }
}

void unread_char(char c)
{
    *--read_ptr = c;
}

void read_space()
{
    char c = '\0';
    while (read_char(&c) && naive_isspace(c));
    if (c) unread_char(c);
}

int read_string(char *s)
{
    int flag = 0;
    char c = '\0';
    read_space();
    while (read_char(&c) && !naive_isspace(c)) {
        *s++ = c;
        flag = 1;
    }
    if (c) unread_char(c);
    if (flag) *s = '\0';
    return flag;
}

int read_llint(long long *lldp)
{
    int flag = 0;
    long long lld = 0;
    int f = 0;
    char c = '\0';
    read_space();
    read_char(&c);
    if (c == '-') f = 1; else unread_char(c);
    while (read_char(&c) && naive_isdigit(c)) {
        lld = lld * 10 + (c - '0');
        flag = 1;
    }
    if (c) unread_char(c);
    if (flag) *lldp = f ? -lld : lld;
    if (!flag && f) unread_char('-');
    return flag;
}

int read_int(int *dp)
{
    int flag = 0;
    int d = 0;
    int f = 0;
    char c = '\0';
    read_space();
    read_char(&c);
    if (c == '-') f = 1; else unread_char(c);
    while (read_char(&c) && naive_isdigit(c)) {
        d = d * 10 + (c - '0');
        flag = 1;
    }
    if (c) unread_char(c);
    if (flag) *dp = f ? -d : d;
    if (!flag && f) unread_char('-');
    return flag;
}

int naive_scanf(const char *fmt, ...)
{
    va_list ap;
    long long *lldp;
    int *dp;
    char c, *cp, *s;
    int cnt = 0;
    int flag = 0;
    
    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt == '%') {
            switch (*++fmt) {
                case 's':
                    s = va_arg(ap, char *);
                    cnt += flag = read_string(s);
                    break;
                case 'd':
                    dp = va_arg(ap, int *);
                    cnt += flag = read_int(dp);
                    break;
                case 'c':
                    cp = va_arg(ap, char *);
                    cnt += flag = read_char(cp);
                    break;
                case 'l':
                    if (*++fmt == 'l' && *++fmt == 'd') {
                        lldp = va_arg(ap, long long *);
                        cnt += flag = read_llint(lldp);
                    }
                    break;
                case '%':
                    if (read_char(&c)) flag = (c == '%');
                    else flag = 0;
                    break;
            }
        } else {
            if (read_char(&c)) flag = (c == *fmt);
            else flag = 0;
        }
        if (!flag) goto done;
        fmt++;
    }
done:
    va_end(ap);
    return cnt;
}





int naive_memcmp(void *p1, void *p2, int n)
{
    unsigned char *s1 = p1, *s2 = p2;
    int i;
    for (i = 0; i < n; i++)
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
    return 0;
}

void naive_memset(void *p, char c, int n)
{
    unsigned char *s = p;
    int i;
    for (i = 0; i < n; i++)
        s[i] = c;
}








int program_main();

#define FILLCHAR 'E'

int main()
{
    naive_memset(output_buffer, FILLCHAR, sizeof(output_buffer));    
    program_main();
    finish_output();
    nemu_assert(naive_memcmp(output_buffer, answer_buffer, sizeof(answer_buffer)) == 0);
    nemu_assert(memcmp(output_buffer, answer_buffer, sizeof(answer_buffer)) == 0);
    HIT_GOOD_TRAP;
    return 0;
}

#define main program_main
#define scanf naive_scanf
#define printf naive_printf
#define puts(str) naive_printf("%s\n", (str))
#define putchar(ch) naive_printf("%c", (ch))

/* REAL USER PROGRAM */


#include <string.h>
#define MAXN 999
#define MAP(X) (((X) - 1) % N * M + ((X) - 1) / N + 1)
int p[MAXN * MAXN];

int inv_cnt;
void merge_sort(int *a, int n)
{
    if (n <= 1) return;
    int l = n / 2;
    int r = n - l;
    int *b = a + l;
    merge_sort(a, l);
    merge_sort(b, r);
    int p, q, sz;
    static int c[MAXN * MAXN];
    p = q = sz = 0;
    while (1) {
        if (p >= l && q >= r) break;
        if (p >= l)
            c[sz++] = b[q++];
        else if (q >= r)
            c[sz++] = a[p++];
        else {
            if (a[p] <= b[q])
                c[sz++] = a[p++];
            else {
                c[sz++] = b[q++];
                inv_cnt += l - p;
            }
        }
    }
    memcpy(a, c, sizeof(int) * n);
}
int get_inv(int *a, int n)
{
    /*int ans = 0;
    int i, j;
    for (i = 0; i < n; i++)
        for (j = i + 1; j < n; j++)
            if (a[i] > a[j])
                ans++;*/
    inv_cnt = 0;
    merge_sort(a, n);
    //printf("ans=%d inv_cnt=%d\n", ans, inv_cnt);
    return inv_cnt;
}

int main()
{
    int i, j;
    int M, N;
    while (scanf("%d%d", &M, &N) == 2) {
        if (!N || !M) break;
        if (N % 2 == 1)
            for (i = 0; i < M; i++)
                for (j = 0; j < N; j++)
                    scanf("%d", &p[i * N + j]);
        else
            for (i = 0; i < M; i++)
                for (j = 0; j < N; j++) {
                    int t;
                    scanf("%d", &t);
                    p[j * M + i] = t ? MAP(t) : 0;
                }
        for (i = 0; i < M * N; i++)
            if (p[i] == 0) {
                for (j = i + 1; j < M * N; j++)
                    p[j - 1] = p[j];
                break;
            }
        //for (i = 0; i < M * N - 1; i++)
            //printf("a[%d]=%d\n", i, p[i]);
        int inv = get_inv(p, M * N - 1);
        //printf("inv=%d\n", inv);
        puts(inv % 2 ? "NO" : "YES");
    }
    return 0;
}
