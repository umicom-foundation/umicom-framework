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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntMeasurementCache c;int32_t x=0;umi_ui_ent_measurement_cache_init(&c);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_measurement_cache_set(&c,"row",31,1U)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_measurement_cache_get(&c,"row",&x)||x!=31)return 2;puts("ok");return 0;}
