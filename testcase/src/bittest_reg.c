#include "trap.h"


/*
    note:
        BT base_register, some_offset
        BT [mem_addr_of_base], some_offset
    they are very different!
*/


int naive_bt_l(int base, int offset)
{
	unsigned char val = offset;
    val &= 31;
	return !!(base & (1 << val));
}

int naive_bt_w(int base, int offset)
{
	unsigned char val = offset;
    val &= 15;
	return !!(base & (1 << val));
}

int asm_bt_reg2r_l(int base, int offset)
{
    int ret = 0;
    __asm__ __volatile__ (
    "clc\n\t"
    "mov %1, %%eax\n\t"
    "mov %2, %%ecx\n\t"
    
    "bt %%ecx, %%eax\n\t"
    
    "setc %%al\n\t"
    "movzbl %%al, %%eax\n\t"
    "mov %%eax, %0\n\t"
    
    :"=m"(ret)
    :"m"(base), "m"(offset)
    :"eax", "ecx");
    
    return ret;
}

int asm_bt_reg2r_w(int base, int offset)
{
    int ret = 0;
    __asm__ __volatile__ (
    "clc\n\t"
    "mov %1, %%eax\n\t"
    "mov %2, %%ecx\n\t"
    
    "bt %%cx, %%ax\n\t"
    
    "setc %%al\n\t"
    "movzbl %%al, %%eax\n\t"
    "mov %%eax, %0\n\t"
    
    :"=m"(ret)
    :"m"(base), "m"(offset)
    :"eax", "ecx");
    
    return ret;
}

int data[] = {
    0, 1, -1, -2, 2, 0x7fffffff, 0x80000000,
    0x19961996, 0xabcdefab, 0x14285714,
    0xdeadbeef, 0xaabbccdd, 0xeeffeeff,
};

int len = sizeof(data) / sizeof(data[0]);

int main()
{
    int i, base, offset;
    int ret_l;
    int ret_reg2r_l;
    int ret_w;
    int ret_reg2r_w;
    
    for (i = 0; i < len; i++) {
        base = data[i];
        for (offset = -100; offset <= 100; offset++) {
            ret_l = naive_bt_l(base, offset);
            ret_reg2r_l = asm_bt_reg2r_l(base, offset);
            nemu_assert(ret_l == ret_reg2r_l);
            
            ret_w = naive_bt_w(base, offset);
            ret_reg2r_w = asm_bt_reg2r_w(base, offset);
            nemu_assert(ret_w == ret_reg2r_w);
        }
    }
    
    HIT_GOOD_TRAP;
    return 0;
}
