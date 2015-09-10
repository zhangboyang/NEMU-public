char input_buffer[] = 
"\x34\x20\x34\x0A\x73\x2E\x2E\x2E\x0A\x2E"
"\x2E\x2E\x2E\x0A\x2E\x2E\x2E\x2E\x0A\x2E"
"\x2E\x2E\x2E\x0A\x32\x20\x33\x0A\x73\x2E"
"\x2E\x0A\x2E\x2E\x2E\x0A\x32\x20\x33\x0A"
"\x73\x78\x78\x0A\x78\x78\x2E\x0A\x36\x20"
"\x36\x0A\x33\x32\x2E\x2E\x2E\x2E\x0A\x34"
"\x31\x2E\x2E\x2E\x2E\x0A\x73\x2E\x2E\x2E"
"\x2E\x2E\x0A\x2E\x2E\x2E\x2E\x2E\x2E\x0A"
"\x2E\x2E\x2E\x2E\x33\x32\x0A\x2E\x2E\x2E"
"\x2E\x34\x31\x0A\x36\x20\x36\x0A\x33\x32"
"\x78\x78\x78\x78\x0A\x34\x31\x78\x78\x78"
"\x78\x0A\x73\x2E\x2E\x2E\x32\x33\x0A\x2E"
"\x2E\x2E\x2E\x31\x34\x0A\x2E\x2E\x2E\x2E"
"\x78\x78\x0A\x2E\x2E\x2E\x2E\x78\x78\x0A"
"\x34\x20\x34\x0A\x32\x33\x78\x78\x0A\x31"
"\x34\x78\x78\x0A\x73\x2E\x34\x31\x0A\x2E"
"\x2E\x33\x32\x0A\x32\x20\x36\x0A\x33\x32"
"\x73\x2E\x33\x32\x0A\x34\x31\x2E\x2E\x34"
"\x31\x0A\x34\x20\x34\x0A\x33\x32\x34\x31"
"\x0A\x34\x31\x33\x32\x0A\x73\x2E\x2E\x2E"
"\x0A\x2E\x2E\x2E\x2E\x0A\x36\x20\x34\x0A"
"\x33\x32\x34\x31\x0A\x34\x31\x33\x32\x0A"
"\x73\x2E\x2E\x2E\x0A\x2E\x2E\x2E\x2E\x0A"
"\x2E\x2E\x2E\x2E\x0A\x2E\x2E\x2E\x2E\x0A"
"\x34\x20\x34\x0A\x33\x32\x2E\x2E\x0A\x34"
"\x31\x2E\x2E\x0A\x73\x2E\x31\x34\x0A\x2E"
"\x2E\x32\x33\x0A\x0A"
;

char answer_buffer[] = 
"\x4E\x6F\x20\x53\x6F\x6C\x75\x74\x69\x6F"
"\x6E\x0A\x4E\x6F\x20\x53\x6F\x6C\x75\x74"
"\x69\x6F\x6E\x0A\x53\x6F\x6C\x75\x74\x69"
"\x6F\x6E\x3A\x20\x28\x31\x2C\x31\x29\x20"
"\x28\x32\x2C\x33\x29\x20\x0A\x53\x6F\x6C"
"\x75\x74\x69\x6F\x6E\x3A\x20\x28\x33\x2C"
"\x31\x29\x20\x28\x35\x2C\x32\x29\x20\x28"
"\x36\x2C\x34\x29\x20\x28\x35\x2C\x36\x29"
"\x20\x28\x31\x2C\x32\x29\x20\x28\x33\x2C"
"\x33\x29\x20\x28\x34\x2C\x31\x29\x20\x28"
"\x36\x2C\x32\x29\x20\x28\x35\x2C\x34\x29"
"\x20\x28\x36\x2C\x36\x29\x20\x28\x32\x2C"
"\x32\x29\x20\x28\x34\x2C\x33\x29\x20\x28"
"\x35\x2C\x31\x29\x20\x28\x36\x2C\x33\x29"
"\x20\x28\x35\x2C\x35\x29\x20\x28\x31\x2C"
"\x31\x29\x20\x28\x33\x2C\x32\x29\x20\x28"
"\x34\x2C\x34\x29\x20\x28\x36\x2C\x35\x29"
"\x20\x28\x32\x2C\x31\x29\x20\x28\x31\x2C"
"\x33\x29\x20\x28\x32\x2C\x35\x29\x20\x28"
"\x34\x2C\x36\x29\x20\x28\x33\x2C\x34\x29"
"\x20\x28\x34\x2C\x32\x29\x20\x28\x36\x2C"
"\x31\x29\x20\x28\x35\x2C\x33\x29\x20\x28"
"\x34\x2C\x35\x29\x20\x28\x32\x2C\x36\x29"
"\x20\x28\x31\x2C\x34\x29\x20\x28\x33\x2C"
"\x35\x29\x20\x28\x31\x2C\x36\x29\x20\x28"
"\x32\x2C\x34\x29\x20\x28\x33\x2C\x36\x29"
"\x20\x28\x31\x2C\x35\x29\x20\x28\x32\x2C"
"\x33\x29\x20\x0A\x53\x6F\x6C\x75\x74\x69"
"\x6F\x6E\x3A\x20\x28\x33\x2C\x31\x29\x20"
"\x28\x35\x2C\x32\x29\x20\x28\x36\x2C\x34"
"\x29\x20\x28\x34\x2C\x35\x29\x20\x28\x32"
"\x2C\x32\x29\x20\x28\x34\x2C\x31\x29\x20"
"\x28\x36\x2C\x32\x29\x20\x28\x35\x2C\x34"
"\x29\x20\x28\x33\x2C\x35\x29\x20\x28\x31"
"\x2C\x32\x29\x20\x28\x33\x2C\x33\x29\x20"
"\x28\x32\x2C\x31\x29\x20\x28\x34\x2C\x36"
"\x29\x20\x28\x33\x2C\x34\x29\x20\x28\x34"
"\x2C\x32\x29\x20\x28\x36\x2C\x31\x29\x20"
"\x28\x35\x2C\x33\x29\x20\x28\x33\x2C\x32"
"\x29\x20\x28\x31\x2C\x31\x29\x20\x28\x33"
"\x2C\x36\x29\x20\x28\x34\x2C\x34\x29\x20"
"\x28\x36\x2C\x33\x29\x20\x28\x35\x2C\x31"
"\x29\x20\x28\x34\x2C\x33\x29\x20\x0A\x4E"
"\x6F\x20\x53\x6F\x6C\x75\x74\x69\x6F\x6E"
"\x0A\x4E\x6F\x20\x53\x6F\x6C\x75\x74\x69"
"\x6F\x6E\x0A\x4E\x6F\x20\x53\x6F\x6C\x75"
"\x74\x69\x6F\x6E\x0A\x4E\x6F\x20\x53\x6F"
"\x6C\x75\x74\x69\x6F\x6E\x0A\x4E\x6F\x20"
"\x53\x6F\x6C\x75\x74\x69\x6F\x6E\x0A"
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


/*马图是哈密顿图？*/

#define assert nemu_assert
#include <string.h>
#include <ctype.h>

#define MAXN 20
char a[MAXN + 1][MAXN + 2];
char b[MAXN + 1][MAXN + 1];
int tx[MAXN + 1][MAXN + 1];
int ty[MAXN + 1][MAXN + 1];
int x[MAXN * MAXN + 1];
int y[MAXN * MAXN + 1];
int pcnt;
int sx, sy;
int n, m;

int u[] = {1, 2, 2, 1, -1, -2, -2, -1};
int v[] = {-2, -1, 1, 2, 2, 1, -1, -2};

int inrange(int x, int y)
{
    return 1 <= x && x <= n && 1 <= y && y <= m;
}

/*int cnt = 0;*/
int search(int curx, int cury, int z, int ld)
{
    /*printf("search(%d, %d, %d)\n", curx, cury, z);*/
    int i, j, nextx, nexty;
    int d = isdigit((int)a[curx][cury]);
    x[z] = curx, y[z] = cury;
    b[curx][cury] = 1;
    for (i = 0; i < 8; i++) {
        if (d && !ld) {
            nextx = tx[curx][cury];
            nexty = ty[curx][cury];
            i = 10;
        } else {
            nextx = curx + u[i];
            nexty = cury + v[i];
        }
        //printf("try(%d,%d)\n", nextx, nexty);
        if (!inrange(nextx, nexty)) continue;
        /*printf("try(%d,%d)\n", nextx, nexty);*/
        /*printf("pcnt=%d z=%d\n", pcnt, z);*/
        if (nextx == sx && nexty == sy && z >= pcnt - 1) {
            /*printf("z=%d %d\n", z, ++cnt);*/
            printf("Solution: ");
            for (j = 0; j <= z; j++)
                printf("(%d,%d) ", x[j], y[j]);
            printf("\n");
            return 1;
        }
        if (!b[nextx][nexty]) {
            if (search(nextx, nexty, z + 1, d))
                return 1;
        }
    }
    b[curx][cury] = 0;
    return 0;
}

void find_trans(int x, int y, int *tx, int *ty)
{
    int i, j;
    for (i = 1; i <= n; i++)
        for (j = 1; j <= m; j++) {
            if (i == x && j == y) continue;
            if (a[x][y] == a[i][j]) {
                *tx = i, *ty = j;
                /*printf("%d %d -> %d %d\n", x, y, i, j);*/
                return;
            }
        }
    assert(0);
}

int main()
{
    int i, j;
    while (scanf("%d%d", &n, &m) == 2) {
        memset(b, 0, sizeof(b));
        pcnt = 0;
        sx = sy = 0;
        for (i = 1; i <= n; i++) {
            scanf("%s", a[i] + 1);
            assert(strlen(a[i] + 1) == m);
        }
        for (i = 1; i <= n; i++) {
            for (j = 1; j <= m; j++) {
                if (isdigit((int)a[i][j]))
                    find_trans(i, j, &tx[i][j], &ty[i][j]);
                if (a[i][j] == 's')
                    sx = i, sy = j;
                if (a[i][j] == 'x') {
                    pcnt--;
                    b[i][j] = 1;
                }
            }
        }
        pcnt += n * m;
        assert(sx > 0 && sy > 0);
        if (!search(sx, sy, 0, 0)) puts("No Solution");
    }
    return 0;
}
