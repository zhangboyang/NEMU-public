#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

enum { R_ES, R_CS, R_SS, R_DS, R_FS, R_GS };

/* DONE: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
    union {
	    union {
		    uint32_t _32;
		    uint16_t _16;
		    uint8_t _8[2];
	    } gpr[8];

	    /* Do NOT change the order of the GPRs' definitions. */

	    struct {
	        uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
	    };
	    
	    struct {
	        uint32_t EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI;
	    };
	    
	    struct {
	        unsigned int AL : 8;
	        unsigned int AH : 8;
	        unsigned int    : 16;
	        unsigned int CL : 8;
	        unsigned int CH : 8;
	        unsigned int    : 16;
	        unsigned int DL : 8;
	        unsigned int DH : 8;
	        unsigned int    : 16;
	        unsigned int BL : 8;
	        unsigned int BH : 8;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	        unsigned int    : 16;
	    };
	    
	    struct {
	        unsigned int AX : 16;
	        unsigned int    : 16;
	        unsigned int CX : 16;
	        unsigned int    : 16;
	        unsigned int DX : 16;
	        unsigned int    : 16;
	        unsigned int BX : 16;
	        unsigned int    : 16;
	        unsigned int SP : 16;
	        unsigned int    : 16;
	        unsigned int BP : 16;
	        unsigned int    : 16;
	        unsigned int SI : 16;
	        unsigned int    : 16;
	        unsigned int DI : 16;
	        unsigned int    : 16;
	    };
	    
	    struct {
	        unsigned int ax : 16;
	        unsigned int    : 16;
	        unsigned int cx : 16;
	        unsigned int    : 16;
	        unsigned int dx : 16;
	        unsigned int    : 16;
	        unsigned int bx : 16;
	        unsigned int    : 16;
	        unsigned int sp : 16;
	        unsigned int    : 16;
	        unsigned int bp : 16;
	        unsigned int    : 16;
	        unsigned int si : 16;
	        unsigned int    : 16;
	        unsigned int di : 16;
	        unsigned int    : 16;
	    };
	};
	
	union EFLAGS_U {
	    uint32_t EFLAGS;
	    uint32_t eflags;
	    struct {
	        unsigned int CF : 1;
	        unsigned int    : 1; // should be 1
	        unsigned int PF : 1;
	        unsigned int    : 1;
	        unsigned int AF : 1;
	        unsigned int    : 1;
	        unsigned int ZF : 1;
	        unsigned int SF : 1;
	        unsigned int TF : 1;
	        unsigned int IF : 1;
	        unsigned int DF : 1;
	        unsigned int OF : 1;
	        unsigned int IOPL : 2;
	        unsigned int NT : 1;
	        unsigned int    : 1;
	        unsigned int RF : 1;
	        unsigned int VM : 1;
	    };
	} EFLAGS;
    #ifdef INVF_CHECK
    union EFLAGS_U EFLAGS_INVALID;
    #endif

    union {
    	swaddr_t eip;
    	swaddr_t EIP;
    };

#ifdef USE_VERY_FAST_MEMORY    
    uint32_t fast_data_base;
    char *fast_data_ptr;
#endif
    
    // don't touch my eip
    int protect_eip; // bit0: don't add instr_len to eip;     bit1: push orig_eip + instr_len to stack
    bool INTR;
    long long executed_instr_count;
    uint32_t orig_eip;
    
    union {
        uint32_t CR[4];
        struct {
            uint32_t CR0, CR1, CR2, CR3;
        };
        struct {
            struct { // CR0
	            unsigned int PE : 1;
	            unsigned int MP : 1;
	            unsigned int EM : 1;
	            unsigned int TS : 1;
	            unsigned int ET : 1;
	            unsigned int    : 26;
	            unsigned int PG : 1;
            };
        };
    };
    
    uint32_t GDTR, GDT_LIMIT;
    uint32_t IDTR, IDT_LIMIT;
    
    union {
        uint16_t seg_reg[6];
        struct {
            uint16_t ES, CS, SS, DS, FS, GS;
        };
    };
    
    uint32_t seg_base[6];
    uint32_t seg_limit[6];
    
    int seg_is_flat;

    int seg_gs_prefix;
    
} CPU_state;

extern CPU_state cpu, old_cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

/* OF, SF, ZF, PF, CF */
#define ALU_EFLAGS_MASK 0x8c5
#define ALU_EFLAGS_MASK_NOCF 0x8c4

#ifdef INVF_CHECK
    /* set flag value */
    #define WRITEF(NAME, VAL) (cpu.EFLAGS.NAME = (VAL), cpu.EFLAGS_INVALID.NAME = 0)
    /* set flag to invalid */
    #define INVF(NAME) (cpu.EFLAGS_INVALID.NAME = 1)
    /* read flag */
    #define READF(NAME) ({ \
        if (cpu.EFLAGS_INVALID.NAME) { \
            printf(c_purple c_bold "%s : %d\n" c_normal, __FILE__, __LINE__); \
            printf(c_purple c_bold "  EIP = 0x%08X\n" c_normal, cpu.EIP); \
            panic("  read invalid flag: " str(NAME)); \
        } \
        cpu.EFLAGS.NAME; \
    })
    #define MERGEF(VAL) ((cpu.EFLAGS_INVALID.EFLAGS &= ~ALU_EFLAGS_MASK), (cpu.EFLAGS.EFLAGS = (cpu.EFLAGS.EFLAGS & (~ALU_EFLAGS_MASK)) | ((VAL) & ALU_EFLAGS_MASK)))
    #define MERGEF_NOCF(VAL) ((cpu.EFLAGS_INVALID.EFLAGS &= ~ALU_EFLAGS_MASK_NOCF), (cpu.EFLAGS.EFLAGS = (cpu.EFLAGS.EFLAGS & (~ALU_EFLAGS_MASK_NOCF)) | ((VAL) & ALU_EFLAGS_MASK_NOCF)))
#else
    #define WRITEF(NAME, VAL) (cpu.EFLAGS.NAME = (VAL))
    #define INVF(NAME) do { } while (0)
    #define READF(NAME) (cpu.EFLAGS.NAME)
    #define MERGEF(VAL) (cpu.EFLAGS.EFLAGS = (cpu.EFLAGS.EFLAGS & (~ALU_EFLAGS_MASK)) | ((VAL) & ALU_EFLAGS_MASK))
    #define MERGEF_NOCF(VAL) (cpu.EFLAGS.EFLAGS = (cpu.EFLAGS.EFLAGS & (~ALU_EFLAGS_MASK_NOCF)) | ((VAL) & ALU_EFLAGS_MASK_NOCF))
#endif



#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsL[];
extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];
extern const char* seg_regs[];

#endif
