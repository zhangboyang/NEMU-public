#include "common.h"
#include <string.h>

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
#define DISK_DATA_OFFSET (1048576)

static const file_info file_table[] = {
	{"1.rpg", 188864, DISK_DATA_OFFSET + 1048576}, 
	{"2.rpg", 188864, DISK_DATA_OFFSET + 1237440},
	{"3.rpg", 188864, DISK_DATA_OFFSET + 1426304}, 
	{"4.rpg", 188864, DISK_DATA_OFFSET + 1615168},
	{"5.rpg", 188864, DISK_DATA_OFFSET + 1804032}, 
	{"abc.mkf", 1022564, DISK_DATA_OFFSET + 1992896},
	{"ball.mkf", 134704, DISK_DATA_OFFSET + 3015460}, 
	{"data.mkf", 66418, DISK_DATA_OFFSET + 3150164},
	{"desc.dat", 16027, DISK_DATA_OFFSET + 3216582}, 
	{"fbp.mkf", 1128064, DISK_DATA_OFFSET + 3232609},
	{"fire.mkf", 834728, DISK_DATA_OFFSET + 4360673}, 
	{"f.mkf", 186966, DISK_DATA_OFFSET + 5195401},
	{"gop.mkf", 11530322, DISK_DATA_OFFSET + 5382367}, 
	{"map.mkf", 1496578, DISK_DATA_OFFSET + 16912689},
	{"mgo.mkf", 1577442, DISK_DATA_OFFSET + 18409267}, 
	{"m.msg", 188232, DISK_DATA_OFFSET + 19986709},
	{"mus.mkf", 331284, DISK_DATA_OFFSET + 20174941}, 
	{"pat.mkf", 8488, DISK_DATA_OFFSET + 20506225},
	{"rgm.mkf", 453202, DISK_DATA_OFFSET + 20514713}, 
	{"rng.mkf", 4546074, DISK_DATA_OFFSET + 20967915},
	{"sss.mkf", 557004, DISK_DATA_OFFSET + 25513989}, 
	{"voc.mkf", 1997044, DISK_DATA_OFFSET + 26070993},
	{"wor16.asc", 5374, DISK_DATA_OFFSET + 28068037}, 
	{"wor16.fon", 82306, DISK_DATA_OFFSET + 28073411},
	{"word.dat", 5650, DISK_DATA_OFFSET + 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

typedef struct {
	int opened;
	uint32_t offset;
} Fstate;

#define FD_START (3)
#define FD_LIMIT (NR_FILES + FD_START) // make space for stdin, out, err
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

int fs_open(const char *pathname, int flags)
{
    int fd;
    for (fd = FD_START; fd < FD_LIMIT; fd++)
        if (strcmp(pathname, FILE_TABLE_BY_FD(fd).name) == 0)
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
