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
typedef struct UmiNativeIrModule { char module_id[UMI_NC_ID_CAPACITY]; char target_triple[UMI_NC_NAME_CAPACITY]; UmiNativeIrFunction functions[UMI_NC_MAX_IR_FUNCTIONS]; size_t function_count; uint64_t revision; } UmiNativeIrModule;
UmiStatus umi_nc_ir_module_init(UmiNativeIrModule *module,const char *module_id,const char *target_triple);
UmiStatus umi_nc_ir_module_add_function(UmiNativeIrModule *module,const UmiNativeIrFunction *function);
UmiNativeIrFunction *umi_nc_ir_module_find(UmiNativeIrModule *module,const char *name);
const UmiNativeIrFunction *umi_nc_ir_module_find_const(const UmiNativeIrModule *module,const char *name);
#ifdef __cplusplus
}
#endif
#endif
