/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/native/optimization_pass.h
 *
 * PURPOSE:
 *   Describe deterministic optimization pass metadata and minimum optimization levels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef enum UmiNativeOptimizationPassKind { UMI_NC_PASS_CONSTANT_FOLD=1, UMI_NC_PASS_DCE=2, UMI_NC_PASS_COPY_PROP=3, UMI_NC_PASS_BRANCH_SIMPLIFY=4 } UmiNativeOptimizationPassKind;
typedef struct UmiNativeOptimizationPass { char id[UMI_NC_ID_CAPACITY]; UmiNativeOptimizationPassKind kind; UmiNativeOptimizationLevel minimum_level; bool repeatable; } UmiNativeOptimizationPass;
UmiStatus umi_nc_optimization_pass_init(UmiNativeOptimizationPass *pass,const char *id,UmiNativeOptimizationPassKind kind,UmiNativeOptimizationLevel minimum_level,bool repeatable);
bool umi_nc_optimization_pass_enabled(const UmiNativeOptimizationPass *pass,const UmiNativeOptimizationProfile *profile);
#ifdef __cplusplus
}
#endif
#endif
