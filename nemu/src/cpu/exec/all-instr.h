#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/movzx.h"
#include "data-mov/movsx.h"
#include "data-mov/xchg.h"
#include "data-mov/push.h"
#include "data-mov/pop.h"
#include "data-mov/cwd_cdq.h"
#include "data-mov/cbw_cwde.h"
#include "data-mov/cmovcc.h"
#include "data-mov/pushad.h"
#include "data-mov/popad.h"
#include "data-mov/cmpxchg.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
#include "arith/sub.h"
#include "arith/sbb.h"
#include "arith/cmp.h"
#include "arith/adc.h"
#include "arith/add.h"
#include "arith/leave.h"

#include "ctl-trans/call.h"
#include "ctl-trans/jcc.h" // jmp is part of jcc
#include "ctl-trans/ret.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shld.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "logic/test.h"
#include "logic/setcc.h"
#include "logic/bt.h"
#include "logic/bsf.h"
#include "logic/bsr.h"
#include "logic/rol.h"

#include "flag-ctl/flag-ctl.h"

#include "string/rep.h"
#include "string/stos.h"
#include "string/movs.h"
#include "string/cmps.h"
#include "string/scas.h"

#include "misc/misc.h"
#include "special/fake_fpu.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "mman/mov.h"
#include "mman/lgdt.h"
#include "mman/lidt.h"
#include "mman/jmpfar.h"

#include "intr/int.h"
#include "intr/iretd.h"
#include "intr/hlt.h"

#include "io/out.h"
#include "io/in.h"

#include "special/special.h"
#include "special/zby.h"
#include "special/gsinstr.h"
#include "special/fake_fpu.h"

