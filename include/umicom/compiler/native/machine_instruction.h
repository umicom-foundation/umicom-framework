/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/machine_instruction.h
 *
 * PURPOSE:
 *   Represent selected target-neutral machine operations before textual assembly or object encoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACHINE_INSTRUCTION_H
#define UMICOM_COMPILER_NATIVE_MACHINE_INSTRUCTION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/machine_operand.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_MACHINE_OPERANDS 4U
typedef enum UmiNativeMachineOpcode { UMI_NC_MOPC_NOP=0, UMI_NC_MOPC_MOV=1, UMI_NC_MOPC_ADD=2, UMI_NC_MOPC_SUB=3, UMI_NC_MOPC_MUL=4, UMI_NC_MOPC_DIV=5, UMI_NC_MOPC_LOAD=6, UMI_NC_MOPC_STORE=7, UMI_NC_MOPC_CMP=8, UMI_NC_MOPC_JMP=9, UMI_NC_MOPC_JCC=10, UMI_NC_MOPC_CALL=11, UMI_NC_MOPC_RET=12 } UmiNativeMachineOpcode;
typedef struct UmiNativeMachineInstruction { uint32_t id; UmiNativeMachineOpcode opcode; UmiNativeMachineOperand operands[UMI_NC_MAX_MACHINE_OPERANDS]; size_t operand_count; bool side_effect; } UmiNativeMachineInstruction;
UmiStatus umi_nc_machine_instruction_init(UmiNativeMachineInstruction *instruction,uint32_t id,UmiNativeMachineOpcode opcode);
UmiStatus umi_nc_machine_instruction_add_operand(UmiNativeMachineInstruction *instruction,UmiNativeMachineOperand operand);
const char *umi_nc_machine_opcode_name(UmiNativeMachineOpcode opcode);
#ifdef __cplusplus
}
#endif
#endif
