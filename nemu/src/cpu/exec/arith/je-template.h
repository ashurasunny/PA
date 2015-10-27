
#include "cpu/exec/template-start.h"

#define instr je

static void do_execute () {
if(cpu.EFLAGS.ZF == 1)
{
    #if DATA_BYTE == 2
         cpu.eip = (cpu.eip + op_src->val)&0x0000ffff;
    #else
         cpu.eip += op_src->val;
    #endif // DATA_BYTE
}
	print_asm_template1();
}

make_instr_helper(i)
//make_instr_helper(rm)

#include "cpu/exec/template-end.h"
