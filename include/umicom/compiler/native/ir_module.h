/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/ir_module.h
 *
 * PURPOSE:
 *   Aggregate native IR functions under a target triple and deterministic module revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_IR_MODULE_H
#define UMICOM_COMPILER_NATIVE_IR_MODULE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/ir_function.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the native ir module data shared with callers of this public contract.
 */
typedef struct UmiNativeIrModule { char module_id[UMI_NC_ID_CAPACITY]; char target_triple[UMI_NC_NAME_CAPACITY]; UmiNativeIrFunction functions[UMI_NC_MAX_IR_FUNCTIONS]; size_t function_count; uint64_t revision; } UmiNativeIrModule;
/**
 * Initialise nc ir module from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_nc_ir_module_init(UmiNativeIrModule *module,const char *module_id,const char *target_triple);
/**
 * Provide the nc ir module add function operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_ir_module_add_function(UmiNativeIrModule *module,const UmiNativeIrFunction *function);
/**
 * Find nc ir module while leaving the underlying catalogue or model owned by this module.
 */
UmiNativeIrFunction *umi_nc_ir_module_find(UmiNativeIrModule *module,const char *name);
/**
 * Provide the nc ir module find const operation used by this module and its client
 * applications.
 */
const UmiNativeIrFunction *umi_nc_ir_module_find_const(const UmiNativeIrModule *module,const char *name);
#ifdef __cplusplus
}
#endif
#endif
