/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_optimization_pass.c
 *
 * PURPOSE:
 *   Regression coverage for describe deterministic optimization pass metadata and minimum optimization levels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/optimization_pass.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeOptimizationPass p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_optimization_pass_init(&p,"fold",UMI_NC_PASS_CONSTANT_FOLD,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile o=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_nc_optimization_pass_enabled(&p,&o))return 2;return 0;}
