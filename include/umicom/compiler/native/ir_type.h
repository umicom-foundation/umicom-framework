/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_type.h
 *
 * PURPOSE:
 *   Describe compact native intermediate-representation scalar and pointer types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiNativeIrType { UmiNativeIrTypeKind kind; uint16_t bits; uint16_t lanes; } UmiNativeIrType;
UmiNativeIrType umi_nc_ir_type_make(UmiNativeIrTypeKind kind,uint16_t bits,uint16_t lanes);
size_t umi_nc_ir_type_size(const UmiNativeIrType *type,size_t pointer_size);
bool umi_nc_ir_type_equal(const UmiNativeIrType *a,const UmiNativeIrType *b);
bool umi_nc_ir_type_is_integer(const UmiNativeIrType *type);
#ifdef __cplusplus
}
#endif
#endif
