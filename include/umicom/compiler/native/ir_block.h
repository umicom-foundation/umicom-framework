/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_block.h
 *
 * PURPOSE:
 *   Store ordered IR instructions and control-flow edges for a basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_BLOCK_H
#define UMICOM_COMPILER_NATIVE_IR_BLOCK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_instruction.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_IR_EDGES 16U
typedef struct UmiNativeIrBlock { uint32_t id; char name[UMI_NC_NAME_CAPACITY]; UmiNativeIrInstruction instructions[UMI_NC_MAX_IR_INSTRUCTIONS]; size_t instruction_count; uint32_t successors[UMI_NC_MAX_IR_EDGES]; size_t successor_count; } UmiNativeIrBlock;
UmiStatus umi_nc_ir_block_init(UmiNativeIrBlock *block,uint32_t id,const char *name);
UmiStatus umi_nc_ir_block_append(UmiNativeIrBlock *block,const UmiNativeIrInstruction *instruction);
UmiStatus umi_nc_ir_block_add_successor(UmiNativeIrBlock *block,uint32_t block_id);
bool umi_nc_ir_block_terminated(const UmiNativeIrBlock *block);
#ifdef __cplusplus
}
#endif
#endif
