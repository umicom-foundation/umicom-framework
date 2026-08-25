/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/function_decl.h
 *
 * PURPOSE:
 *   Model native function signatures, parameter type IDs, variadic state and ABI-relevant flags.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiNativeFunctionDecl { char name[UMI_NC_NAME_CAPACITY]; uint32_t return_type_id; uint32_t parameter_type_ids[UMI_NC_MAX_PARAMETERS]; size_t parameter_count; bool variadic; bool noreturn_function; bool inline_hint; } UmiNativeFunctionDecl;
UmiStatus umi_nc_function_decl_init(UmiNativeFunctionDecl *decl,const char *name,uint32_t return_type_id);
UmiStatus umi_nc_function_decl_add_parameter(UmiNativeFunctionDecl *decl,uint32_t type_id);
uint64_t umi_nc_function_decl_signature(const UmiNativeFunctionDecl *decl);
#ifdef __cplusplus
}
#endif
#endif
