/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/macro.h
 *
 * PURPOSE:
 *   Describe object-like and function-like preprocessor macros with stable fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_MACRO_H
#define UMICOM_COMPILER_NATIVE_MACRO_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_MACRO_PARAMETERS 16U
/**
 * Represent the native macro data shared with callers of this public contract.
 */
typedef struct UmiNativeMacro { char name[UMI_NC_NAME_CAPACITY]; char replacement[UMI_NC_TEXT_CAPACITY]; char parameters[UMI_NC_MAX_MACRO_PARAMETERS][UMI_NC_NAME_CAPACITY]; size_t parameter_count; bool function_like; bool variadic; uint64_t fingerprint; } UmiNativeMacro;
/**
 * Initialise nc macro from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_macro_init(UmiNativeMacro *macro,const char *name,const char *replacement,bool function_like);
/**
 * Provide the nc macro add parameter operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_macro_add_parameter(UmiNativeMacro *macro,const char *name);
/**
 * Check that nc macro satisfies its contract before another service relies on it.
 */
UmiStatus umi_nc_macro_validate(const UmiNativeMacro *macro);
/**
 * Provide the nc macro compute fingerprint operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_macro_compute_fingerprint(const UmiNativeMacro *macro);
#ifdef __cplusplus
}
#endif
#endif
