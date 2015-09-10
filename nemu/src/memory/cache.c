#include "memory/memory.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef USE_MEMORY_CACHE

static void read_nextlevel_l2(void *dest, unsigned addr, int len)
{
    int u32_cnt = len / 4;
    int u8_cnt = len % 4;
    uint32_t *u32_dest = dest;
    uint8_t *u8_dest = dest;
    
    int i;
    for (i = 0; i < u32_cnt; i++)
        u32_dest[i] = hwaddr_read_nocache(addr + i * 4, 4);
    for (i = 0; i < u8_cnt; i++)
        u8_dest[u32_cnt * 4 + i] = hwaddr_read_nocache(addr + u32_cnt * 4 + i, 1);
}

static void write_nextlevel_l2(unsigned addr, void *src, int len)
{
    int u32_cnt = len / 4;
    int u8_cnt = len % 4;
    uint32_t *u32_src = src;
    uint8_t *u8_src = src;
    
    int i;
    for (i = 0; i < u32_cnt; i++)
        hwaddr_write_nocache(addr + i * 4, 4, u32_src[i]);
    for (i = 0; i < u8_cnt; i++)
        hwaddr_write_nocache(addr + u32_cnt * 4 + i, 1, u8_src[u32_cnt * 4 + i]);
}


// generate code for L2 Cache
#define CACHE_PREFIX l2
#define READ_NEXTLEVEL read_nextlevel_l2
#define WRITE_NEXTLEVEL write_nextlevel_l2
#define ADDR_SIZE 32
#define CACHE_S 4
#define CACHE_B 6
#define CACHE_E 4096
#define CACHE_WRITEBACK
#define CACHE_PERFCOUNTER
#include "cache-template.h"



static void read_nextlevel_l1(void *dest, unsigned addr, int len)
{
    l2cache_read(dest, addr, len);
}

static void write_nextlevel_l1(unsigned addr, void *src, int len)
{
    l2cache_write(addr, src, len);
}


// generate code for L1 Cache
#define CACHE_PREFIX l1
#define READ_NEXTLEVEL read_nextlevel_l1
#define WRITE_NEXTLEVEL write_nextlevel_l1
#define ADDR_SIZE 32
#define CACHE_S 3
#define CACHE_B 6
#define CACHE_E 128
#define CACHE_WRITETHROUGH
#define CACHE_PERFCOUNTER
#include "cache-template.h"

#endif
