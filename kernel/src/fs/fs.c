#include "common.h"
#include <string.h>

typedef struct {
	char name[20];
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
#define DISK_DATA_OFFSET (512 * 4096)
#define FILE_TABLE_LINE_SIZE 16
#define MAX_FILES 100
static file_info file_table[MAX_FILES];
static int fs_init_flag = 0;

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

typedef struct {
	int opened;
	uint32_t offset;
} Fstate;

#define FD_START (3)
#define FD_LIMIT (MAX_FILES + FD_START) // make space for stdin, out, err
#define FILE_STATE_BY_FD(fd) (file_state[(fd) - FD_START])
#define FILE_TABLE_BY_FD(fd) (file_table[(fd) - FD_START])

static Fstate file_state[FD_LIMIT];

static void fs_rawread(void *buf, unsigned raw_offset, int len)
{
    //printk("fs: rawread(%p, 0x%08x, %d)\n", buf, raw_offset, len);
    ide_read(buf, raw_offset, len);
}

static void fs_rawwrite(unsigned raw_offset, void *buf, int len)
{
    ide_write(buf, raw_offset, len);
}


void fs_init()
{
    unsigned offset = DISK_DATA_OFFSET;
    
    int nr_files = 0;
    unsigned totsz = 0;
    
    memset(file_table, 0, sizeof(file_table));
    
    char buf[FILE_TABLE_LINE_SIZE + 1] = {};
    while (1) {
        // read filename part from file table
        fs_rawread(buf, offset, FILE_TABLE_LINE_SIZE);
        offset += FILE_TABLE_LINE_SIZE;
        char str[FILE_TABLE_LINE_SIZE + 1];
        strcpy(str, strtok(buf, " "));

        if (strcmp(str, "END_OF_LIST") == 0) break;
        
        // read filesize part from file table
        fs_rawread(buf, offset, FILE_TABLE_LINE_SIZE);
        offset += FILE_TABLE_LINE_SIZE;
        unsigned sz = 0;
        char *ptr;
        for (ptr = buf; *ptr && *ptr != ' '; ptr++)
            sz = sz * 10 + *ptr - '0';
        
        if (nr_files < MAX_FILES) {
            strcpy(file_table[nr_files].name, str);
            file_table[nr_files].size = sz;
            file_table[nr_files].disk_offset = totsz; // need fix later
            totsz += sz;
            nr_files++;
            
            printk("fs_init(): name = %s, size = 0x%08x\n", str, sz);
        }
    }
    
    // fix disk_offset
    int i;
    for (i = 0; i < nr_files; i++) {
        file_table[i].disk_offset += offset;
    }
    
    fs_init_flag = 1;
}

int fs_open(const char *pathname, int flags)
{
    if (!fs_init_flag) fs_init();
    int fd;
    for (fd = FD_START; fd < FD_LIMIT; fd++)
        if (strcasecmp(pathname, FILE_TABLE_BY_FD(fd).name) == 0)
            break;
    assert(fd < FD_LIMIT);

    assert(!FILE_STATE_BY_FD(fd).opened);
    
    FILE_STATE_BY_FD(fd).opened = 1;
    FILE_STATE_BY_FD(fd).offset = 0;
    printk("syscall: open(\"%s\", %d) = %d\n", pathname, flags, fd);
    return fd;
}

int fs_read(int fd, void *buf, int len)
{   
    //printk("syscall: read(%d, %p, %d)\n", fd, buf, len);
    assert(FD_START <= fd && fd < FD_LIMIT);
    assert(FILE_STATE_BY_FD(fd).opened);
    assert(FILE_STATE_BY_FD(fd).offset <= FILE_TABLE_BY_FD(fd).size);
    assert(len >= 0);
    assert(FILE_STATE_BY_FD(fd).offset + len >= FILE_STATE_BY_FD(fd).offset);
    if (FILE_STATE_BY_FD(fd).offset + len > FILE_TABLE_BY_FD(fd).size) {
        len = FILE_TABLE_BY_FD(fd).size - FILE_STATE_BY_FD(fd).offset;
    }
    unsigned raw_offset = FILE_TABLE_BY_FD(fd).disk_offset + FILE_STATE_BY_FD(fd).offset;
    fs_rawread(buf, raw_offset, len);
    FILE_STATE_BY_FD(fd).offset += len;
    return len;
}


int fs_write(int fd, void *buf, int len)
{
    assert(FD_START <= fd && fd < FD_LIMIT);
    assert(FILE_STATE_BY_FD(fd).opened);
    assert(FILE_STATE_BY_FD(fd).offset <= FILE_TABLE_BY_FD(fd).size);
    assert(len >= 0);
    assert(FILE_STATE_BY_FD(fd).offset + len >= FILE_STATE_BY_FD(fd).offset);
    if (FILE_STATE_BY_FD(fd).offset + len > FILE_TABLE_BY_FD(fd).size) {
        len = FILE_TABLE_BY_FD(fd).size - FILE_STATE_BY_FD(fd).offset;
    }
    unsigned raw_offset = FILE_TABLE_BY_FD(fd).disk_offset + FILE_STATE_BY_FD(fd).offset;
    fs_rawwrite(raw_offset, buf, len);
    FILE_STATE_BY_FD(fd).offset += len;
    return len;
}


int fs_lseek(int fd, int offset, int whence)
{
    assert(FD_START <= fd && fd < FD_LIMIT);
    assert(FILE_STATE_BY_FD(fd).opened);
    
    unsigned new_offset = 0;
    switch (whence) {
        case 0: //SEEK_SET
            new_offset = offset;
            break;
        case 1: //SEEK_CUR
            new_offset = FILE_STATE_BY_FD(fd).offset + offset;
            break;
        case 2: //SEEK_END
            new_offset = FILE_TABLE_BY_FD(fd).size + offset;
            break;
        default:
            panic("unknown whence %d", whence);
    }
    assert(new_offset >= 0);
    assert(new_offset <= FILE_TABLE_BY_FD(fd).size);
    FILE_STATE_BY_FD(fd).offset = new_offset;
    return FILE_STATE_BY_FD(fd).offset;
}

int fs_close(int fd)
{
    assert(FD_START <= fd && fd < FD_LIMIT);
    printk("syscall: close(%d) [fn = %s]\n", fd, FILE_TABLE_BY_FD(fd).name);
    FILE_STATE_BY_FD(fd).opened = 0;
    return 0;
}
