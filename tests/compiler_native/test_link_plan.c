/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_link_plan.c
 *
 * PURPOSE:
 *   Regression coverage for build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/link_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiNativeLinkPlan p;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_init(&p,"app",UMI_NC_LINK_EXECUTABLE)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_add_input(&p,"main.o")!=UMI_STATUS_OK||umi_nc_link_plan_add_library(&p,"m")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_nc_link_plan_fingerprint(&p)==0U)return 3;return 0;}
