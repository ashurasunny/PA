#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute () {
    DATA_TYPE result = op_dest->val & op_src->val;

    /* TODO: Update cpu.EFLAGS. */
    cpu.EFLAGS.ZF = (result == 0);
    cpu.EFLAGS.CF = 0;
    cpu.EFLAGS.OF = 0;
  //  cpu.EFLAGS.SF = (signed)result < 0 ? 1 : 0;
    if((signed)result > 0)
        cpu.EFLAGS.SF = 0;
    else cpu.EFLAGS.SF = 1;
    uint8_t plowerbyte = result & 0xff;
    cpu.EFLAGS.PF = (plowerbyte&00000001)^((plowerbyte&00000010)>>1)^((plowerbyte&00000100)>>2)^((plowerbyte&00001000)>>3)^((plowerbyte&00010000)>>4)^((plowerbyte&00100000)>>5)^((plowerbyte&01000000)>>6)^((plowerbyte&10000000)>>7);
   cpu.EFLAGS.PF = 1;
   // panic("please implement me");

    print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"

