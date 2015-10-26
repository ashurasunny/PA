
#include "cpu/exec/template-start.h"

#define instr call

static void do_execute () {
   if(2 == DATA_BYTE)
    {
       // cpu.esp-=2;
    //    MEM_W(cpu.esp,cpu.eip);
       // cpu.eip = (cpu.eip+op_src->val)&0x0000ffff;
    }
 if(4 == DATA_BYTE)
    {
        cpu.esp-=4;
      MEM_W(cpu.esp,cpu.eip);
        cpu.eip += op_src->val;
    }
   /***** cpu.esp -= 4;
    MEM_W(cpu.esp,cpu.eip);
    #if DATA_BYTE == 2
         cpu.eip = (cpu.eip + op_src->val)&0x0000ffff;
    #else
         cpu.eip += op_src->val;
    #endif // DATA_BYTE
	print_asm_template1();****/
}

make_instr_helper(i)
//make_instr_helper(rm)

#include "cpu/exec/template-end.h"

