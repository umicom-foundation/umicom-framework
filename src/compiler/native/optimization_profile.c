/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/optimization_profile.c
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
#include "umicom/compiler/native/optimization_profile.h"
UmiNativeOptimizationProfile umi_nc_optimization_profile_default(UmiNativeOptimizationLevel level){UmiNativeOptimizationProfile p={0};p.level=level;p.preserve_debug=level==UMI_NC_OPT_O0;p.vectorize=level==UMI_NC_OPT_O3;p.inline_functions=level>=UMI_NC_OPT_O2&&level<=UMI_NC_OPT_O3;p.max_pass_iterations=level==UMI_NC_OPT_O0?1U:(level==UMI_NC_OPT_O1?2U:4U);return p;}
