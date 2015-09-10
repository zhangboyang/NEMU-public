//        T    S      B
//  addr [tag][index][offset]

#define CACHE_T (ADDR_SIZE - (CACHE_S) - (CACHE_B))
#define CACHE_BLOCK_SIZE (1 << (CACHE_B))
#define CACHE_LINE_COUNT (CACHE_E)
#define CACHE_SET_COUNT (1 << (CACHE_S))

#define get_tag(addr) ((addr) >> (CACHE_S) >> (CACHE_B))
#define get_index(addr) (((addr) >> (CACHE_B)) & ((1 << (CACHE_S)) - 1))
#define get_offset(addr) (((addr) & ((1 << (CACHE_B)) - 1)))

#define round_to_block(addr) ((addr) & ((~0) << (CACHE_B)))

#define min(a, b) ({ typeof(a) __a = (a); typeof(b) __b = (b); __a < __b ? __a : __b; })

#define CACHE_TAG concat(CACHE_PREFIX, cache_tag_t)
#define ADDR concat(CACHE_PREFIX, addr_t)
#define WORD concat(CACHE_PREFIX, word_t)

#define CACHE_LINE concat(CACHE_PREFIX, cache_line_t)
#define CACHE_SET concat(CACHE_PREFIX, cache_set_t)
#define CACHE_STRUCT concat(CACHE_PREFIX, cache_t)
#define CACHE_COUNTER concat(CACHE_PREFIX, cache_counter_t)

#define CACHE_DATA concat(CACHE_PREFIX, cache_data)
#define CACHE_COUNTER_DATA concat(CACHE_PREFIX, cache_counter_data)

#define MOVE_CACHE_LINE_TO_FRONT concat(CACHE_PREFIX, cache_move_line_to_front)
#define FIND_CACHE_LINE concat(CACHE_PREFIX, cache_find_line)
#define ALLOC_CACHE_LINE concat(CACHE_PREFIX, cache_alloc_line)
#define READ_CACHE concat(CACHE_PREFIX, cache_read)
#define WRITE_CACHE concat(CACHE_PREFIX, cache_write)
#define SHOW_CACHELINE concat(CACHE_PREFIX, cache_show_cacheline)
#define SHOW_PERFCOUNTER concat(CACHE_PREFIX, cache_show_perfcounter)
#define INIT_CACHE concat(CACHE_PREFIX, cache_init)


typedef unsigned CACHE_TAG;
typedef unsigned ADDR;

typedef struct {
    CACHE_TAG tag;
    int valid;
#ifdef CACHE_WRITEBACK
    int dirty;    
#endif
    char data[CACHE_BLOCK_SIZE];
#ifdef DEBUG
    int canary;
#endif
} CACHE_LINE;

typedef struct {
    CACHE_LINE line[CACHE_LINE_COUNT];
} CACHE_SET;

typedef struct {
    CACHE_SET set[CACHE_SET_COUNT];
} CACHE_STRUCT;

static CACHE_STRUCT CACHE_DATA;


#ifdef CACHE_PERFCOUNTER
typedef struct {
    long long r[2]; // r[0] = read miss, r[1] = read hit
    long long w[2];
} CACHE_COUNTER;
static CACHE_COUNTER CACHE_COUNTER_DATA;
#endif



static void MOVE_CACHE_LINE_TO_FRONT(CACHE_LINE array[], int index)
{
    // move found to front for efficiency
    // 0, 1, 2, ..., i  , i+1, i+2
    // i, 0, 1, ..., i-1, i+1, i+2
    CACHE_LINE tmp = array[index];
    memmove(&array[1], &array[0], sizeof(CACHE_LINE) * index);
    array[0] = tmp;
}

static CACHE_LINE *FIND_CACHE_LINE(int index, CACHE_TAG tag)
{
    CACHE_SET *set = &CACHE_DATA.set[index];
    int i;
    for (i = 0; i < CACHE_LINE_COUNT; i++) {
        if (set->line[i].valid && set->line[i].tag == tag) {
            MOVE_CACHE_LINE_TO_FRONT(set->line, i);
            return &set->line[0];
        }
        #ifdef DEBUG
        assert(set->line[i].canary == 0);
        #endif
    }
    return NULL;
}

static CACHE_LINE *ALLOC_CACHE_LINE(int index, CACHE_TAG tag)
{
    CACHE_SET *set = &CACHE_DATA.set[index];
    int i;
    for (i = 0; i < CACHE_LINE_COUNT; i++) {
        if (!set->line[i].valid) {
            MOVE_CACHE_LINE_TO_FRONT(set->line, i);
            return &set->line[0];
        }
        #ifdef DEBUG
        assert(set->line[i].canary == 0);
        #endif
    }
    //return &set->line[rand() % CACHE_LINE_COUNT];
    return &set->line[CACHE_LINE_COUNT - 1];
}


    
#define READ_NEXTLEVEL_LINE(dst, addr) (READ_NEXTLEVEL((dst)->data, round_to_block((addr)), CACHE_BLOCK_SIZE))
#define WRITE_NEXTLEVEL_LINE(dst, addr) (WRITE_NEXTLEVEL(round_to_block((addr)), (dst)->data, CACHE_BLOCK_SIZE))

#ifdef CACHE_WRITETHROUGH
    #define FREE_LINE(index, line) do { } while (0)
#endif

#ifdef CACHE_WRITEBACK
    #define FREE_LINE(index, line) do { \
        CACHE_LINE *__line_to_free = (line); \
        if (__line_to_free->valid && __line_to_free->dirty) { \
            ADDR __addr = ((index) | (__line_to_free->tag << CACHE_S)) << CACHE_B; \
            WRITE_NEXTLEVEL(__addr, __line_to_free->data, CACHE_BLOCK_SIZE); \
        } \
    } while (0)
#endif

#define LOAD_LINE(addr, index, tag) ({ \
    int __index = (index); \
    CACHE_TAG __tag = (tag); \
    CACHE_LINE *__line = ALLOC_CACHE_LINE(__index, __tag); \
    FREE_LINE(__index, __line); \
    __line->tag = tag; \
    __line->valid = 1; \
    READ_NEXTLEVEL_LINE(__line, (addr)); \
    __line; \
})

void READ_CACHE(void *dest, ADDR addr, int len)
{
    while (len > 0) {
        int index = get_index(addr);
        int offset = get_offset(addr);
        CACHE_TAG tag = get_tag(addr);
        int block_len = min(CACHE_BLOCK_SIZE - offset, len);
        CACHE_LINE *line = FIND_CACHE_LINE(index, tag);
#ifdef CACHE_PERFCOUNTER
        CACHE_COUNTER_DATA.r[!!line]++;
#endif
        if (!line) { // miss
            line = LOAD_LINE(addr, index, tag);
        }
        memcpy(dest, line->data + offset, block_len);
        #ifdef DEBUG
        assert(line == NULL || line->canary == 0);
        #endif
        len -= block_len;
        addr += block_len;
        dest += block_len;
    }
}

void WRITE_CACHE(ADDR addr, void *src, int len)
{
    while (len > 0) {
        int index = get_index(addr);
        int offset = get_offset(addr);
        CACHE_TAG tag = get_tag(addr);
        int block_len = min(CACHE_BLOCK_SIZE - offset, len);
        CACHE_LINE *line = FIND_CACHE_LINE(index, tag);
#ifdef CACHE_PERFCOUNTER
        CACHE_COUNTER_DATA.w[!!line]++;
#endif
#ifdef CACHE_WRITETHROUGH
        if (line) { // hit
            memcpy(line->data + offset, src, block_len);
            WRITE_NEXTLEVEL_LINE(line, addr);
        } else { // miss
            WRITE_NEXTLEVEL(addr, src, len);
        }
#endif
#ifdef CACHE_WRITEBACK
        if (!line) { // miss
            line = LOAD_LINE(addr, index, tag);
        }
        memcpy(line->data + offset, src, block_len);
        line->dirty = 1;
#endif
        #ifdef DEBUG
        assert(line == NULL || line->canary == 0);
        #endif
        len -= block_len;
        addr += block_len;
        src += block_len;
    }
}

void INIT_CACHE()
{
    assert(CACHE_T > 0);
    assert(CACHE_BLOCK_SIZE > 0);
    
    memset(&CACHE_DATA, 0, sizeof(CACHE_DATA));
}

void SHOW_CACHELINE(ADDR addr)
{
    int index = get_index(addr);
    int offset = get_offset(addr);
    CACHE_TAG tag = get_tag(addr);
    CACHE_LINE *line = FIND_CACHE_LINE(index, tag);
    printf("  addr=%08x index=%08x offset=%08x tag=%08x\n", addr, index, offset, tag);
    if (line) {
        printf("  valid=%d", line->valid);
        #ifdef CACHE_WRITEBACK
        printf("  dirty=%d", line->dirty);
        #endif
        printf("\n");
        printf("    ");
        int i;
        int j = 0;
        int lc = 16;
        for (i = 0; i < CACHE_BLOCK_SIZE; i++) {
            if (i == offset) printf(c_red c_bold);
            printf("%02x ", (unsigned) (unsigned char) line->data[i]);
            if (i == offset) printf(c_normal);
            if (i % lc == lc - 1 || i == CACHE_BLOCK_SIZE - 1) {
                printf("|");
                while (j <= i) {
                    printf("%c", isprint(line->data[j]) ? line->data[j] : '.');
                    j++;
                }
                printf("|\n    ");
            }
        }
        printf("\n");
    } else {
        printf("  no line found\n");
    }
}

#ifdef CACHE_PERFCOUNTER
void SHOW_PERFCOUNTER()
{
    printf("  ways = %d (S = %d)\n", CACHE_SET_COUNT, CACHE_S);
    printf("  lines (E) = %d\n", CACHE_E);
    printf("  block size = %d (B = %d)\n", CACHE_BLOCK_SIZE, CACHE_B);
    int sz = (1 << (CACHE_S + CACHE_B)) * CACHE_E;
    printf("  total size = %d bytes (%.2f KB, %.2f MB)\n", sz, sz / 1024.0, sz / 1048576.0);
    printf("\n");
    printf("%9s %16s %16s\n", "r/w", "hit", "miss");
    printf("%9s %16lld %16lld\n", "read", CACHE_COUNTER_DATA.r[1], CACHE_COUNTER_DATA.r[0]);
    printf("%9s %16lld %16lld\n", "write", CACHE_COUNTER_DATA.w[1], CACHE_COUNTER_DATA.w[0]);
    printf("\n");
    printf("  total time = %lld\n", (CACHE_COUNTER_DATA.r[1] + CACHE_COUNTER_DATA.w[1]) * 2 + (CACHE_COUNTER_DATA.r[0] + CACHE_COUNTER_DATA.w[0]) * 200);
}
#endif

#ifdef CACHE_WRITETHROUGH
#undef CACHE_WRITETHROUGH
#endif

#ifdef CACHE_WRITEBACK
#undef CACHE_WRITEBACK
#endif

#ifdef CACHE_PERFCOUNTER
#undef CACHE_PERFCOUNTER
#endif

#undef CACHE_T
#undef CACHE_BLOCK_SIZE
#undef CACHE_LINE_COUNT
#undef CACHE_SET_COUNT

#undef get_tag
#undef get_index
#undef get_offset
#undef round_to_block

#undef min

#undef CACHE_TAG
#undef ADDR
#undef WORD

#undef CACHE_LINE
#undef CACHE_SET
#undef CACHE_STRUCT
#undef CACHE_COUNTER

#undef CACHE_DATA
#undef CACHE_COUNTER_DATA

#undef MOVE_CACHE_LINE_TO_FRONT
#undef FIND_CACHE_LINE
#undef READ_CACHE
#undef WRITE_CACHE
#undef SHOW_CACHELINE
#undef SHOW_PERFCOUNTER
#undef INIT_CACHE

#undef READ_NEXTLEVEL_LINE
#undef WRITE_NEXTLEVEL_LINE
#undef FREE_LINE
#undef LOAD_LINE

#undef CACHE_PREFIX
#undef READ_NEXTLEVEL
#undef WRITE_NEXTLEVEL
#undef ADDR_SIZE
#undef CACHE_S
#undef CACHE_B
#undef CACHE_E
