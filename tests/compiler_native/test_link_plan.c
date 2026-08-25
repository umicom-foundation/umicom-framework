/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_native/test_link_plan.c
 *
 * PURPOSE:
 *   Regression coverage for build deterministic native link plans from objects, libraries, search roots and output kind.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/link_plan.h"
int main(void){UmiNativeLinkPlan p;if(umi_nc_link_plan_init(&p,"app",UMI_NC_LINK_EXECUTABLE)!=UMI_STATUS_OK)return 1;if(umi_nc_link_plan_add_input(&p,"main.o")!=UMI_STATUS_OK||umi_nc_link_plan_add_library(&p,"m")!=UMI_STATUS_OK)return 2;if(umi_nc_link_plan_fingerprint(&p)==0U)return 3;return 0;}
