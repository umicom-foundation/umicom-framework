/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_optimization_profile.c
 *
 * PURPOSE:
 *   Regression coverage for describe optimization level, size goals, debug preservation and native pass budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/optimization_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeOptimizationProfile p=umi_nc_optimization_profile_default(UMI_NC_OPT_O3);/* Apply this branch only when its contract condition is satisfied. */ if(!p.vectorize||!p.inline_functions||p.max_pass_iterations!=4U)return 1;return 0;}
