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
/**
 * Represent the native ir function data shared with callers of this public contract.
 */
typedef struct UmiNativeIrFunction { char name[UMI_NC_NAME_CAPACITY]; UmiNativeIrType return_type; UmiNativeIrBlock blocks[UMI_NC_MAX_IR_BLOCKS]; size_t block_count; uint32_t entry_block_id; uint32_t next_block_id; uint32_t next_instruction_id; uint32_t next_value_id; } UmiNativeIrFunction;
/**
 * Initialise nc ir function from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_ir_function_init(UmiNativeIrFunction *function,const char *name,UmiNativeIrType return_type);
/**
 * Provide the nc ir function add block operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_function_add_block(UmiNativeIrFunction *function,const char *name,uint32_t *out_block_id);
/**
 * Provide the nc ir function block operation used by this module and its client
 * applications.
 */
UmiNativeIrBlock *umi_nc_ir_function_block(UmiNativeIrFunction *function,uint32_t id);
/**
 * Provide the nc ir function block const operation used by this module and its client
 * applications.
 */
const UmiNativeIrBlock *umi_nc_ir_function_block_const(const UmiNativeIrFunction *function,uint32_t id);
/**
 * Provide the nc ir function next value operation used by this module and its client
 * applications.
 */
uint32_t umi_nc_ir_function_next_value(UmiNativeIrFunction *function);
/**
 * Provide the nc ir function next instruction operation used by this module and its client
 * applications.
 */
uint32_t umi_nc_ir_function_next_instruction(UmiNativeIrFunction *function);
#ifdef __cplusplus
}
#endif
#endif
