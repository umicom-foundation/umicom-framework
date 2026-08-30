/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_function.h
 *
 * PURPOSE:
 *   Own IR basic blocks and stable value/block ID allocation for one native function.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_FUNCTION_H
#define UMICOM_COMPILER_NATIVE_IR_FUNCTION_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_block.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeIrFunction { char name[UMI_NC_NAME_CAPACITY]; UmiNativeIrType return_type; UmiNativeIrBlock blocks[UMI_NC_MAX_IR_BLOCKS]; size_t block_count; uint32_t entry_block_id; uint32_t next_block_id; uint32_t next_instruction_id; uint32_t next_value_id; } UmiNativeIrFunction;
UmiStatus umi_nc_ir_function_init(UmiNativeIrFunction *function,const char *name,UmiNativeIrType return_type);
UmiStatus umi_nc_ir_function_add_block(UmiNativeIrFunction *function,const char *name,uint32_t *out_block_id);
UmiNativeIrBlock *umi_nc_ir_function_block(UmiNativeIrFunction *function,uint32_t id);
const UmiNativeIrBlock *umi_nc_ir_function_block_const(const UmiNativeIrFunction *function,uint32_t id);
uint32_t umi_nc_ir_function_next_value(UmiNativeIrFunction *function);
uint32_t umi_nc_ir_function_next_instruction(UmiNativeIrFunction *function);
#ifdef __cplusplus
}
#endif
#endif
