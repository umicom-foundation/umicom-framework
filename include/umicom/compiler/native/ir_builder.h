/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_builder.h
 *
 * PURPOSE:
 *   Emit constants, arithmetic and return instructions into the current IR basic block.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_BUILDER_H
#define UMICOM_COMPILER_NATIVE_IR_BUILDER_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiNativeIrBuilder { UmiNativeIrFunction *function; UmiNativeIrBlock *block; } UmiNativeIrBuilder;
UmiStatus umi_nc_ir_builder_begin(UmiNativeIrBuilder *builder,UmiNativeIrFunction *function,uint32_t block_id);
UmiStatus umi_nc_ir_builder_const_i64(UmiNativeIrBuilder *builder,int64_t value,uint32_t *out_value_id);
UmiStatus umi_nc_ir_builder_binary(UmiNativeIrBuilder *builder,UmiNativeIrOpcode opcode,uint32_t left,uint32_t right,UmiNativeIrType type,uint32_t *out_value_id);
UmiStatus umi_nc_ir_builder_return(UmiNativeIrBuilder *builder,uint32_t value_id);
#ifdef __cplusplus
}
#endif
#endif
