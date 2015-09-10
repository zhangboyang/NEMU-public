#ifndef USE_ASM_EFLAGS

// =============== C-BASED EFLAGS MACROS =================

/* EFLAGS_BASE(a, b, c, p, u) : base implement
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS_BASE(op1, op2, cin, patch, updatecf) ({ \
    uint32_t __cin, __patch;                                                \
    uint32_t __a, __b, __c, __ch, __as, __bs, __cs, __cc, __p;              \
    __cin = !!(cin);                                                        \
    __patch = !!(patch);                                                    \
    __a = ((uint32_t)(op1)) & MASK;                                         \
    __b = ((uint32_t)(op2)) & MASK;                                         \
    if (__cin) { __b = ~__b & MASK; }                                       \
    __c = (__a + __b + (__cin ^ __patch)) & MASK;                           \
    __ch = (__a + __b) & MASK;                                              \
    __cc = (__ch < __a || __ch < __b || __c < __ch);                        \
    __as = __a >> SIGN_SHIFT;                                               \
    __bs = __b >> SIGN_SHIFT;                                               \
    __cs = __c >> SIGN_SHIFT;                                               \
    __p = __c ^ (__c >> 4);                                                 \
    __p = __p ^ (__p >> 2);                                                 \
    __p = (__p ^ (__p >> 1)) & 1;                                           \
                                                                            \
    WRITEF(ZF, (__c == 0));                                                 \
    WRITEF(SF, __cs);                                                       \
    WRITEF(OF, (__as == __bs && __as != __cs));                             \
    if (updatecf) WRITEF(CF, __cc ^ __cin);                                 \
    WRITEF(PF, __p ^ 1);                                                    \
                                                                            \
    __c;                                                                    \
})

/* EFLAGS_ALU(a, b, c, p) : same as EFLAGS(), but p is used by ADC and SBB
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS_ALU(a, b, c, p) EFLAGS_BASE((a), (b), (c), (p), 1)

/* EFLAGS(a, b, c) : if c then return a-b; else return a+b;
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS(op1, op2, cin) EFLAGS_ALU((op1), (op2), (cin), 0)

/* EFLAGS_B(result) set eflags for AND, OR, XOR ...
            changes PF, SF, ZF; clear CF, OF
 */
#define EFLAGS_B(result) ({ \
    uint32_t __c, __cs, __p;                                                \
    __c = ((uint32_t)(result)) & MASK;                                      \
    __cs = __c >> SIGN_SHIFT;                                               \
    __p = __c ^ (__c >> 4);                                                 \
    __p = __p ^ (__p >> 2);                                                 \
    __p = (__p ^ (__p >> 1)) & 1;                                           \
                                                                            \
    WRITEF(ZF, (__c == 0));                                                 \
    WRITEF(SF, __cs);                                                       \
    WRITEF(PF, __p ^ 1);                                                    \
    WRITEF(OF, 0);                                                          \
    WRITEF(CF, 0);                                                          \
                                                                            \
    __c;                                                                    \
})


#else

#undef EFLAGS_ASM_INSTR
#undef EFLAGS_ASM_OP

#if DATA_BYTE == 1
#define EFLAGS_ASM_INSTR(instr) instr "b "
#define EFLAGS_ASM_OP(op) "%b" op
#elif DATA_BYTE == 2
#define EFLAGS_ASM_INSTR(instr) instr "w "
#define EFLAGS_ASM_OP(op) "%w" op
#elif DATA_BYTE == 4
#define EFLAGS_ASM_INSTR(instr) instr "l "
#define EFLAGS_ASM_OP(op) "%k" op
#else
#error unknown DATA_BYTE
#endif


// =============== ASM-BASED EFLAGS MACROS =================

/* EFLAGS_BASE(a, b, c, p, u) : base implement
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS_BASE(op1, op2, cin, patch, updatecf) ({ \
    unsigned long __op1 = (op1); /* also result value */ \
    unsigned long __op2 = (op2); /* also result flags */ \
    \
    if ((updatecf) == 0) { /* instruction is INC, DEC */ \
        if (__op2 != 1 || (patch) != 0) \
            panic("unknown usage of EFLAG_BASE"); \
        if (!cin) { \
            __asm__ __volatile__ ( \
            EFLAGS_ASM_INSTR("inc") EFLAGS_ASM_OP("0")"\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
            :"+q"(__op1), "=q"(__op2)); \
        } else { \
            __asm__ __volatile__ ( \
            EFLAGS_ASM_INSTR("dec") EFLAGS_ASM_OP("0")"\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
            :"+q"(__op1), "=q"(__op2)); \
        } \
    } else { /* instruction is ADC, SBB */ \
        unsigned long __patch = (patch); \
        if (!cin) { \
            __asm__ __volatile__ ( \
            "bt $0, %2\n\t" \
            EFLAGS_ASM_INSTR("adc") EFLAGS_ASM_OP("1") "," EFLAGS_ASM_OP("0") "\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
            :"+q"(__op1), "+q"(__op2) \
            :"q"(__patch)); \
        } else { \
            __asm__ __volatile__ ( \
            "bt $0, %2\n\t" \
            EFLAGS_ASM_INSTR("sbb") EFLAGS_ASM_OP("1") "," EFLAGS_ASM_OP("0") "\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
            :"+q"(__op1), "+q"(__op2) \
            :"q"(__patch)); \
        } \
    } \
    if (updatecf) { \
        MERGEF(__op2); \
    } else { \
        MERGEF_NOCF(__op2); \
    } \
    __op1; \
})

/* EFLAGS_ALU(a, b, c, p) : same as EFLAGS(), but p is used by ADC and SBB
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS_ALU(a, b, c, p) EFLAGS_BASE((a), (b), (c), (p), 1)

/* EFLAGS(a, b, c) : if c then return a-b; else return a+b;
                   changes ZF, SF, OF, CF, PF */
#define EFLAGS(op1, op2, cin) ({ \
    unsigned long __op1 = (op1); /* also result value */ \
    unsigned long __op2 = (op2); /* also result eflags */ \
    if (!cin) { \
        __asm__ __volatile__ ( \
            EFLAGS_ASM_INSTR("add") EFLAGS_ASM_OP("1") "," EFLAGS_ASM_OP("0") "\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
        :"+q"(__op1), "+q"(__op2)); \
    } else { \
        __asm__ __volatile__ ( \
            EFLAGS_ASM_INSTR("sub") EFLAGS_ASM_OP("1") "," EFLAGS_ASM_OP("0") "\n\t" \
            "pushf\n\t" \
            "pop %1\n\t" \
        :"+q"(__op1), "+q"(__op2)); \
    } \
    MERGEF(__op2); \
    __op1; \
})

/* EFLAGS_B(result) set eflags for AND, OR, XOR ...
            changes PF, SF, ZF; clear CF, OF
 */
#define EFLAGS_B(result) ({ \
    unsigned long __result = (result); \
    unsigned long __result_eflags; \
    __asm__ __volatile__ ( \
        EFLAGS_ASM_INSTR("test") EFLAGS_ASM_OP("1") "," EFLAGS_ASM_OP("1") "\n\t" \
        "pushf\n\t" \
        "pop %0\n\t" \
    :"=q"(__result_eflags) \
    :"q"(__result)); \
    MERGEF(__result_eflags); \
    __result; \
})

#endif
