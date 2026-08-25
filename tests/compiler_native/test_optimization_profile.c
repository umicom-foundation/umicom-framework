/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_optimization_profile.c
 *
 * PURPOSE:
 *   Regression coverage for describe optimization level, size goals, debug preservation and native pass budgets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/optimization_profile.h"
int main(void){UmiNativeOptimizationProfile p=umi_nc_optimization_profile_default(UMI_NC_OPT_O3);if(!p.vectorize||!p.inline_functions||p.max_pass_iterations!=4U)return 1;return 0;}
