#ifndef __DEBUG_H__
#define __DEBUG_H__

/* for assert() */
#ifndef DEBUG
    #ifndef NDEBUG
        #define NDEBUG
    #endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <setjmp.h>

extern FILE* log_fp;

#ifdef LOG_FILE
#	define Log_write(format, ...) fprintf(log_fp, format, ## __VA_ARGS__), fflush(log_fp)
#else
#	define Log_write(format, ...)
#endif

#define Log(format, ...) \
	do { \
		fprintf(stdout, "\33[1;34m[%s,%d,%s] " format "\33[0m\n", \
				__FILE__, __LINE__, __func__, ## __VA_ARGS__); \
		fflush(stdout); \
		Log_write("[%s,%d,%s] " format "\n", \
				__FILE__, __LINE__, __func__, ## __VA_ARGS__); \
	} while(0)

#define Assert(cond, ...) \
	do { \
		if(!(cond)) { \
			fflush(stdout); \
			fprintf(stderr, "\33[1;31m"); \
			fprintf(stderr, __VA_ARGS__); \
			fprintf(stderr, "\33[0m\n"); \
			assert(cond); \
		} \
	} while(0)

#define panic(format, ...) \
	do { printf(format, ## __VA_ARGS__); putchar('\n'); printf("%s", c_normal); abort(); exit(1); } while (0)

#define panic_to_ui(format, ...) \
	do { \
        printf("\n"); \
        printf("%s", c_red c_bold); \
        printf("  panic at %s:%d:%s\n", __FILE__, __LINE__, __func__); \
        printf("  "); \
        printf(format, ## __VA_ARGS__); \
        printf("%s", c_normal); \
        printf("\n"); \
        \
        int cnt = 0; \
        while (1) { \
            char buf[100]; \
            printf("%s", c_purple); \
            printf("  input 'a' to abort nemu (default)\n"); \
            printf("  input 'u' to go back to ui\n"); \
            printf("%s", c_normal); \
            printf("(panic) "); \
            char *r = fgets(buf, sizeof(buf), stdin); \
            if (strstr(buf, "u")) { \
                extern jmp_buf jbuf; \
                longjmp(jbuf, 2); \
            } \
            if (++cnt >= 3 || !r || !*buf || *buf == '\n' || strstr(buf, "a")) { \
                abort(); \
                exit(1); \
            } \
        } \
    } while (0)

#endif

