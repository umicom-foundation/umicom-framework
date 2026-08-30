/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_enterprise_data_service.c
 *
 * PURPOSE:
 *   Exercise the enterprise data service enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/enterprise_data_service.h"
#include <stdio.h>
int main(void){UmiUiEntEnterpriseDataService s;umi_ui_ent_enterprise_data_service_init(&s);umi_ui_ent_enterprise_data_service_update(&s,1U,1U,1U,100U,20U);if(!umi_ui_ent_enterprise_data_service_ready(&s)||s.active_grids!=1U)return 1;puts("ok");return 0;}
