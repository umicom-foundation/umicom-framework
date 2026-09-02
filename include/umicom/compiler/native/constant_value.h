/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/constant_value.h
 *
 * PURPOSE:
 *   Represent compile-time integer, floating-point and boolean constants without host pointer aliasing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_CONSTANT_VALUE_H
#define UMICOM_COMPILER_NATIVE_CONSTANT_VALUE_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native constant kind values accepted by this public contract.
 */
typedef enum UmiNativeConstantKind { UMI_NC_CONST_INVALID=0, UMI_NC_CONST_SIGNED=1, UMI_NC_CONST_UNSIGNED=2, UMI_NC_CONST_FLOAT=3, UMI_NC_CONST_BOOL=4 } UmiNativeConstantKind;
/**
 * Represent the native constant value data shared with callers of this public contract.
 */
typedef struct UmiNativeConstantValue { UmiNativeConstantKind kind; int64_t signed_value; uint64_t unsigned_value; double float_value; bool bool_value; } UmiNativeConstantValue;
/**
 * Provide the nc constant signed operation used by this module and its client
 * applications.
 */
UmiNativeConstantValue umi_nc_constant_signed(int64_t value);
/**
 * Provide the nc constant unsigned operation used by this module and its client
 * applications.
 */
UmiNativeConstantValue umi_nc_constant_unsigned(uint64_t value);
/**
 * Provide the nc constant bool operation used by this module and its client applications.
 */
UmiNativeConstantValue umi_nc_constant_bool(bool value);
/**
 * Provide the nc constant truthy operation used by this module and its client
 * applications.
 */
bool umi_nc_constant_truthy(const UmiNativeConstantValue *value);
/**
 * Provide the nc constant equal operation used by this module and its client applications.
 */
bool umi_nc_constant_equal(const UmiNativeConstantValue *a,const UmiNativeConstantValue *b);
#ifdef __cplusplus
}
#endif
#endif
