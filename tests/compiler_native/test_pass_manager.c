/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_pass_manager.c
 *
 * PURPOSE:
 *   Regression coverage for build deterministic optimization pipelines and filter passes by optimization profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/pass_manager.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativePassManager m;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_nc_pass_manager_default(&m)!=UMI_STATUS_OK)return 1;UmiNativeOptimizationProfile p=umi_nc_optimization_profile_default(UMI_NC_OPT_O2);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(m.count!=4U||umi_nc_pass_manager_enabled_count(&m,&p)!=4U)return 2;return 0;}
