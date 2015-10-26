#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute () {
    int src,dst,rst;
    uint32_t dest_val = op_dest -> val;
    src = (signed) op_src->val < 0 ? 1 : 0;
    dst = (signed) op_dest->val < 0 ? 1 : 0;
    DATA_TYPE result = op_dest->val - op_src->val;
    OPERAND_W(op_dest, result);
    rst = (signed) result < 0 ? 1 : 0;
    /* TODO: Update cpu.EFLAGS. */
    cpu.EFLAGS.ZF = (result == 0);
    cpu.EFLAGS.CF = result > dest_val;
    cpu.EFLAGS.OF = (src&(!dst)&(!rst)) || ((!src)&dst&rst);
    cpu.EFLAGS.SF = (signed)result < 0 ? 1 : 0;
    cpu.EFLAGS.AF = 0;
    uint8_t plowerbyte = cpu.EFLAGS.PF & 0xff;
  //  cpu.EFLAGS.PF = (result|result>>1|result>>2|result>>3|result>>4|result>>5|result>>6|result>>7)&0x1;
    cpu.EFLAGS.PF = (plowerbyte&00000001)^(plowerbyte&00000010)^(plowerbyte&00000100)^(plowerbyte&00001000)^(plowerbyte&00010000)^(plowerbyte&00100000)^(plowerbyte&01000000)^(plowerbyte&10000000);
   // panic("please implement me");

    print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
