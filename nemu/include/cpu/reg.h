#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef union
    {
        uint32_t val;
        struct
        {
            uint32_t CF : 1;//进位标志
            uint32_t ytw : 1;
            uint32_t PF : 1;//奇偶标志
            uint32_t ytw1 : 1;
            uint32_t AF : 1;//辅助进位标志
            uint32_t ytw2 : 1;
            uint32_t ZF : 1;//零标志
            uint32_t SF : 1;//符号标志
            uint32_t TF : 1;
            uint32_t IF : 1;
            uint32_t DF : 1;
            uint32_t OF : 1;//溢出标志
            uint32_t IOPL : 2;
            uint32_t NT : 1;
            uint32_t ytw3 : 1;
            uint32_t RF : 1;
            uint32_t VM : 1;
        };
    } eflags;

typedef struct
{
    /* Do NOT change the order of the GPRs' definitions. */
    //uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    union
    {
        union
        {
            uint32_t _32;
            uint16_t _16;
            uint8_t _8[2];
        } gpr[8];
        struct
        {
            uint32_t eax;
            uint32_t ecx;
            uint32_t edx;
            uint32_t ebx;
            uint32_t esp;
            uint32_t ebp;
            uint32_t esi;
            uint32_t edi;
        };
    };
    eflags EFLAGS;
    swaddr_t eip;

} CPU_state;

extern CPU_state cpu;


//extern EFLAGS eflags;
static inline int check_reg_index(int index)
{
    assert(index >= 0 && index < 8);
    return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
