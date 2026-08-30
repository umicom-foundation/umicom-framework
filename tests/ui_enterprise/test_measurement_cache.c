/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_measurement_cache.c
 *
 * PURPOSE:
 *   Exercise the measurement cache enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/measurement_cache.h"
#include <stdio.h>
int main(void){UmiUiEntMeasurementCache c;int32_t x=0;umi_ui_ent_measurement_cache_init(&c);if(umi_ui_ent_measurement_cache_set(&c,"row",31,1U)!=UMI_STATUS_OK)return 1;if(!umi_ui_ent_measurement_cache_get(&c,"row",&x)||x!=31)return 2;puts("ok");return 0;}
