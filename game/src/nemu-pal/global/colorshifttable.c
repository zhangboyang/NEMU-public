#include "_common.h"
#include "colorshifttable.h"
static BYTE make_colorshift_table_single(BYTE in, SHORT shift)
{
    BYTE b = (in & 0x0F);

    b += shift;

    if (b & 0x80)
    {
        b = 0;
    }
    else if (b & 0x70)
    {
        b = 0x0F;
    }

    return (b | (in & 0xF0));
}
BYTE cs_table[256];
static SHORT cs_shift = 0x7fff;
VOID make_colorshift_table(SHORT shift)
{
    //ZBY_TIMING_BEGIN(3);
    int i;
    if (shift != cs_shift) {
        for (i = 0; i <= 0xff; i++) {
            cs_table[i] = make_colorshift_table_single(i, shift);
        }
    }
    //ZBY_TIMING_END(3, "make color shift table");
}
