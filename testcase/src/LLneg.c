#include "trap.h"

/* NOTE:
    if you fail this testcase, it's not your fault.
    PA says there's no need to update EFLAGS when executing NEG instruction
    but it's not true!
    
    negating a long long int will be compiled to these instructions:
  800031:	f7 d8                	neg    %eax
  800033:	83 d2 00             	adc    $0x0,%edx // *** ADC uses CF ! ***
  800036:	f7 da                	neg    %edx

    so, please update CF when executing NEG instruction
*/

long long LLneg(long long x)
{
    return -x; // neg, adc
}

int main()
{
    int n = 6;
    long long a[] = {5, 6, 7, -5, -6, -7};
    long long b[] = {-5, -6, -7, 5, 6, 7};
    int i;
    for (i = 0; i < n; i++)
        nemu_assert(LLneg(a[i]) == b[i]);
    HIT_GOOD_TRAP;
    return 0;
}

