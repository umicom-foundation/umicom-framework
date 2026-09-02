/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/optimization_profile.h
 *
 * PURPOSE:
 *   Describe optimization level, size goals, debug preservation and native pass budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_OPTIMIZATION_PROFILE_H
#define UMICOM_COMPILER_NATIVE_OPTIMIZATION_PROFILE_H
#include <stdbool.h>
#include <stddef.h>
#include "umicom/compiler/native/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native optimization level values accepted by this public contract.
 */
typedef enum UmiNativeOptimizationLevel { UMI_NC_OPT_O0=0, UMI_NC_OPT_O1=1, UMI_NC_OPT_O2=2, UMI_NC_OPT_O3=3, UMI_NC_OPT_SIZE=4 } UmiNativeOptimizationLevel;
/**
 * Represent the native optimization profile data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeOptimizationProfile { UmiNativeOptimizationLevel level; bool preserve_debug; bool vectorize; bool inline_functions; size_t max_pass_iterations; } UmiNativeOptimizationProfile;
/**
 * Provide the nc optimization profile default operation used by this module and its client
 * applications.
 */
UmiNativeOptimizationProfile umi_nc_optimization_profile_default(UmiNativeOptimizationLevel level);
#ifdef __cplusplus
}
#endif
#endif
