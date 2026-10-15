/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/optimization_pass.h
 *
 * PURPOSE:
 *   Describe deterministic optimization pass metadata and minimum optimization levels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_NATIVE_OPTIMIZATION_PASS_H
#define UMICOM_COMPILER_NATIVE_OPTIMIZATION_PASS_H
#include <stdbool.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/compiler/native/optimization_profile.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named native optimization pass kind values accepted by this public contract.
 */
typedef enum UmiNativeOptimizationPassKind { UMI_NC_PASS_CONSTANT_FOLD=1, UMI_NC_PASS_DCE=2, UMI_NC_PASS_COPY_PROP=3, UMI_NC_PASS_BRANCH_SIMPLIFY=4 } UmiNativeOptimizationPassKind;
/**
 * Represent the native optimization pass data shared with callers of this public contract.
 */
typedef struct UmiNativeOptimizationPass { char id[UMI_NC_ID_CAPACITY]; UmiNativeOptimizationPassKind kind; UmiNativeOptimizationLevel minimum_level; bool repeatable; } UmiNativeOptimizationPass;
/**
 * Initialise nc optimization pass from caller-provided values so later operations receive
 * a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_nc_optimization_pass_init(UmiNativeOptimizationPass *pass,const char *id,UmiNativeOptimizationPassKind kind,UmiNativeOptimizationLevel minimum_level,bool repeatable);
/**
 * Provide the nc optimization pass enabled operation used by this module and its client
 * applications.
 */
bool umi_nc_optimization_pass_enabled(const UmiNativeOptimizationPass *pass,const UmiNativeOptimizationProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
