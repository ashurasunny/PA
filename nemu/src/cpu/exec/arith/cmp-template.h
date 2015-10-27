#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute () {
    int src,dst,rst;
//    uint32_t dest_val = op_dest -> val;
    src = (signed) op_src->val < 0 ? 1 : 0;
    dst = (signed) op_dest->val < 0 ? 1 : 0;
    DATA_TYPE result = op_dest->val - op_src->val;
    rst = (signed) result < 0 ? 1 : 0;
    /* TODO: Update cpu.EFLAGS. */
    cpu.EFLAGS.ZF = (result == 0);
    if(op_dest->val < op_src->val)
        cpu.EFLAGS.CF = 1;
    else cpu.EFLAGS.CF = 0;
    cpu.EFLAGS.OF = (src&(!dst)&(!rst)) || ((!src)&dst&rst);
  //  cpu.EFLAGS.SF = (signed)result < 0 ? 1 : 0;
    if((signed)result > 0)
        cpu.EFLAGS.SF = 0;
    else cpu.EFLAGS.SF = 1;
    cpu.EFLAGS.AF = 0;
    uint8_t plowerbyte = result & 0xff;
    cpu.EFLAGS.PF = (plowerbyte&00000001)^((plowerbyte&00000010)>>1)^((plowerbyte&00000100)>>2)^((plowerbyte&00001000)>>3)^((plowerbyte&00010000)>>4)^((plowerbyte&00100000)>>5)^((plowerbyte&01000000)>>6)^((plowerbyte&10000000)>>7);
   cpu.EFLAGS.PF = 1;
   // panic("please implement me");

    print_asm_template2();
}


make_instr_helper(i2rm)


#include "cpu/exec/template-end.h"
