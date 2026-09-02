/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/function_decl.h
 *
 * PURPOSE:
 *   Model native function signatures, parameter type IDs, variadic state and ABI-relevant flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_FUNCTION_DECL_H
#define UMICOM_COMPILER_NATIVE_FUNCTION_DECL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_PARAMETERS 32U
/**
 * Represent the native function decl data shared with callers of this public contract.
 */
typedef struct UmiNativeFunctionDecl { char name[UMI_NC_NAME_CAPACITY]; uint32_t return_type_id; uint32_t parameter_type_ids[UMI_NC_MAX_PARAMETERS]; size_t parameter_count; bool variadic; bool noreturn_function; bool inline_hint; } UmiNativeFunctionDecl;
/**
 * Initialise nc function decl from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_function_decl_init(UmiNativeFunctionDecl *decl,const char *name,uint32_t return_type_id);
/**
 * Provide the nc function decl add parameter operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_function_decl_add_parameter(UmiNativeFunctionDecl *decl,uint32_t type_id);
/**
 * Provide the nc function decl signature operation used by this module and its client
 * applications.
 */
uint64_t umi_nc_function_decl_signature(const UmiNativeFunctionDecl *decl);
#ifdef __cplusplus
}
#endif
#endif
