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
int main(void){UmiNativeOptimizationPass p;if(umi_nc_optimization_pass_init(&p,"fold",UMI_NC_PASS_CONSTANT_FOLD,UMI_NC_OPT_O1,true)!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile o=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);if(!umi_nc_optimization_pass_enabled(&p,&o))return 2;return 0;}
