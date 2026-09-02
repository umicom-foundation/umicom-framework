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
/**
 * Represent the native pass manager data shared with callers of this public contract.
 */
typedef struct UmiNativePassManager { UmiNativeOptimizationPass passes[UMI_NC_MAX_OPT_PASSES]; size_t count; } UmiNativePassManager;
/**
 * Initialise nc pass manager from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_pass_manager_init(UmiNativePassManager *manager);
/**
 * Add nc pass manager only after its inputs and available capacity have been checked.
 */
UmiStatus umi_nc_pass_manager_add(UmiNativePassManager *manager,const UmiNativeOptimizationPass *pass);
/**
 * Provide the nc pass manager default operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_pass_manager_default(UmiNativePassManager *manager);
/**
 * Return the number of records represented by nc pass manager enabled without changing
 * their state.
 */
size_t umi_nc_pass_manager_enabled_count(const UmiNativePassManager *manager,const UmiNativeOptimizationProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
