/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/abi/compatibility.h
 *
 * PURPOSE:
 *   Evaluate whether a consumer ABI requirement can run against an available provider ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * ABI files describe agreements between separately compiled code. They help us detect unsafe binary changes before an application is released.
 */
#ifndef UMICOM_ABI_COMPATIBILITY_H
#define UMICOM_ABI_COMPATIBILITY_H
#include <stddef.h>
#include "umicom/abi/descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAbiCompatibility { int compatible; size_t missing_features; size_t missing_symbols; size_t incompatible_layouts; } UmiAbiCompatibility;
UmiStatus umi_abi_compatibility_check(const UmiAbiDescriptor *required, const UmiAbiDescriptor *available, UmiAbiCompatibility *out_result);
#ifdef __cplusplus
}
#endif
#endif
