/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/constant_value.h
 *
 * PURPOSE:
 *   Represent compile-time integer, floating-point and boolean constants without host pointer aliasing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CONSTANT_VALUE_H
#define UMICOM_COMPILER_NATIVE_CONSTANT_VALUE_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiNativeConstantKind { UMI_NC_CONST_INVALID=0, UMI_NC_CONST_SIGNED=1, UMI_NC_CONST_UNSIGNED=2, UMI_NC_CONST_FLOAT=3, UMI_NC_CONST_BOOL=4 } UmiNativeConstantKind;
typedef struct UmiNativeConstantValue { UmiNativeConstantKind kind; int64_t signed_value; uint64_t unsigned_value; double float_value; bool bool_value; } UmiNativeConstantValue;
UmiNativeConstantValue umi_nc_constant_signed(int64_t value);
UmiNativeConstantValue umi_nc_constant_unsigned(uint64_t value);
UmiNativeConstantValue umi_nc_constant_bool(bool value);
bool umi_nc_constant_truthy(const UmiNativeConstantValue *value);
bool umi_nc_constant_equal(const UmiNativeConstantValue *a,const UmiNativeConstantValue *b);
#ifdef __cplusplus
}
#endif
#endif
