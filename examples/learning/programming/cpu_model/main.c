/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/cpu_model/main.c
 *
 * PURPOSE:
 *   A tiny fetch-decode-execute machine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum Opcode { LOAD, ADD, STORE, HALT } Opcode;
typedef struct Instruction { Opcode opcode; unsigned operand; } Instruction;
typedef struct Cpu { unsigned accumulator; unsigned memory[4]; size_t pc; bool halted; } Cpu;
static bool Step(Cpu *cpu, const Instruction *program, size_t count)
{
    if (cpu==NULL || program==NULL || cpu->halted || cpu->pc>=count) return false;
    Instruction instruction = program[cpu->pc];
    Cpu next = *cpu; /* Rejected instructions cannot partially modify the machine. */
    switch (instruction.opcode) {
    case LOAD: next.accumulator=instruction.operand; break;
    case ADD: next.accumulator+=instruction.operand; break; /* unsigned modulo arithmetic */
    case STORE:
        if (instruction.operand>=4U) return false;
        next.memory[instruction.operand]=next.accumulator; break;
    case HALT: next.halted=true; break;
    default: return false;
    }
    ++next.pc; *cpu=next; return true;
}
int main(void)
{
    const Instruction program[]={{LOAD,7U},{ADD,5U},{STORE,2U},{HALT,0U}};
    Cpu cpu={0};
    for (size_t step=0U; step<4U; ++step)
        if (!Step(&cpu,program,4U)) return EXIT_FAILURE;
    if (!cpu.halted || cpu.memory[2]!=12U || Step(&cpu,program,4U)) return EXIT_FAILURE;
    Cpu invalid={0}; const Instruction bad[]={{STORE,4U}};
    if (Step(&invalid,bad,1U) || invalid.pc!=0U) return EXIT_FAILURE;
    puts("LOAD 7 -> ADD 5 -> STORE memory[2] = 12 -> HALT");
    puts("PASS: cpu_model");
    return EXIT_SUCCESS;
}
