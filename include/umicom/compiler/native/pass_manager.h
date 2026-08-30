/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/pass_manager.h
 *
 * PURPOSE:
 *   Build deterministic optimization pipelines and filter passes by optimization profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_PASS_MANAGER_H
#define UMICOM_COMPILER_NATIVE_PASS_MANAGER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/optimization_pass.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_NC_MAX_OPT_PASSES 32U
typedef struct UmiNativePassManager { UmiNativeOptimizationPass passes[UMI_NC_MAX_OPT_PASSES]; size_t count; } UmiNativePassManager;
void umi_nc_pass_manager_init(UmiNativePassManager *manager);
UmiStatus umi_nc_pass_manager_add(UmiNativePassManager *manager,const UmiNativeOptimizationPass *pass);
UmiStatus umi_nc_pass_manager_default(UmiNativePassManager *manager);
size_t umi_nc_pass_manager_enabled_count(const UmiNativePassManager *manager,const UmiNativeOptimizationProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
