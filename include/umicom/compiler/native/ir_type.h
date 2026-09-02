/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_type.h
 *
 * PURPOSE:
 *   Describe compact native intermediate-representation scalar and pointer types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_TYPE_H
#define UMICOM_COMPILER_NATIVE_IR_TYPE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native ir type data shared with callers of this public contract.
 */
typedef struct UmiNativeIrType { UmiNativeIrTypeKind kind; uint16_t bits; uint16_t lanes; } UmiNativeIrType;
/**
 * Provide the nc ir type make operation used by this module and its client applications.
 */
UmiNativeIrType umi_nc_ir_type_make(UmiNativeIrTypeKind kind,uint16_t bits,uint16_t lanes);
/**
 * Return the number of records represented by nc ir type without changing their state.
 */
size_t umi_nc_ir_type_size(const UmiNativeIrType *type,size_t pointer_size);
/**
 * Provide the nc ir type equal operation used by this module and its client applications.
 */
bool umi_nc_ir_type_equal(const UmiNativeIrType *a,const UmiNativeIrType *b);
/**
 * Provide the nc ir type is integer operation used by this module and its client
 * applications.
 */
bool umi_nc_ir_type_is_integer(const UmiNativeIrType *type);
#ifdef __cplusplus
}
#endif
#endif
