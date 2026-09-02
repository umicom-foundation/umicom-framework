/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_aggregation_result.c
 *
 * PURPOSE:
 *   Exercise the aggregation result enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_result.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntAggregationResult r;umi_ui_ent_aggregation_result_init(&r,UMI_UI_ENT_AGG_AVERAGE);umi_ui_ent_aggregation_result_add(&r,2.0);umi_ui_ent_aggregation_result_add(&r,4.0);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_aggregation_result_value(&r)!=3.0)return 1;puts("ok");return 0;}
