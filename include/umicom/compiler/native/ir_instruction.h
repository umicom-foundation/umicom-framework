/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_instruction.h
 *
 * PURPOSE:
 *   Represent typed IR instructions, operands, side effects and terminators in a compact C23 form.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_INSTRUCTION_H
#define UMICOM_COMPILER_NATIVE_IR_INSTRUCTION_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_type.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_IR_OPERANDS 4U
typedef struct UmiNativeIrInstruction { uint32_t id; UmiNativeIrOpcode opcode; uint32_t result_id; UmiNativeIrType result_type; uint32_t operands[UMI_NC_MAX_IR_OPERANDS]; size_t operand_count; int64_t immediate; bool side_effect; } UmiNativeIrInstruction;
UmiStatus umi_nc_ir_instruction_init(UmiNativeIrInstruction *instruction,uint32_t id,UmiNativeIrOpcode opcode,uint32_t result_id,UmiNativeIrType result_type);
UmiStatus umi_nc_ir_instruction_add_operand(UmiNativeIrInstruction *instruction,uint32_t value_id);
bool umi_nc_ir_instruction_is_terminator(const UmiNativeIrInstruction *instruction);
bool umi_nc_ir_instruction_produces_value(const UmiNativeIrInstruction *instruction);
#ifdef __cplusplus
}
#endif
#endif
