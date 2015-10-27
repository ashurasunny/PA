#include "cpu/exec/template-start.h"

#define instr push

static void do_execute () {
   if(2 == DATA_BYTE)
    {
        cpu.esp-=2;
        MEM_W(cpu.esp,op_src->val);
    }
 else if(4 == DATA_BYTE)
    {
    cpu.esp-=4;
      MEM_W(cpu.esp,op_src->val);
    }
    print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
