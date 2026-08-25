/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_pass_manager.c
 *
 * PURPOSE:
 *   Regression coverage for build deterministic optimization pipelines and filter passes by optimization profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/pass_manager.h"
int main(void){UmiNativePassManager m;if(umi_nc_pass_manager_default(&m)!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile p=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);if(m.count!=4U||umi_nc_pass_manager_enabled_count(&m,&p)!=4U)return 2;return 0;}
