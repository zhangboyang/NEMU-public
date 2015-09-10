#include "common.h"
#include "cpu/reg.h"
#include <string.h>
#include <stdlib.h>

#define PROGRAM_START (0x08048000)
#define CODE_SIZE (4 * 1024 * 1024)
#define MAX_FUNC 10000
#define MAX_PERF_DISPLAY 15

static long long record_data[CODE_SIZE];

void perf_record_eip()
{
    unsigned int eip = cpu.EIP;
    if (eip >= PROGRAM_START && eip - PROGRAM_START < CODE_SIZE) {
        record_data[eip - PROGRAM_START]++;
    }
}


struct perf_data {
    const char *func_name;
    unsigned long long count;
};
static struct perf_data func_data[MAX_FUNC];
static int func_count;


static unsigned long long get_count(unsigned eip)
{
    if (eip >= PROGRAM_START && eip - PROGRAM_START < CODE_SIZE) {
        return record_data[eip - PROGRAM_START];
    } else {
        return 0;
    }
}
static void func_callback(const char *name, unsigned start, unsigned len)
{
    if (func_count >= MAX_FUNC) return;
    struct perf_data *data = &func_data[func_count++];
    data->func_name = name;
    while (len--) {
        data->count += get_count(start++);
    }
}

static int cmp_func(const void *a, const void *b)
{
    unsigned long long count_a = ((struct perf_data *) a)->count;
    unsigned long long count_b = ((struct perf_data *) b)->count;
    if (count_a > count_b) return -1;
    if (count_a < count_b) return 1;
    return 0;
}

void enum_functions(void (*callback)(const char *name, unsigned start, unsigned len));


void show_perf_data()
{
    func_count = 0;
    enum_functions(func_callback);
    qsort(func_data, func_count, sizeof(struct perf_data), cmp_func);
    int display_count = MAX_PERF_DISPLAY;
    if (func_count < display_count) display_count = func_count;
    int i;
    printf("  %s%30s %-10s%s\n", c_yellow, "function", "hits", c_normal);
    for (i = 0; i < display_count; i++) {
        printf("  %30s %-10llu\n", func_data[i].func_name, func_data[i].count);
    }
}


void reset_record()
{
    memset(record_data, 0, sizeof(record_data));
    memset(func_data, 0, sizeof(func_data));
}

