/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_aggregation_result.c
 *
 * PURPOSE:
 *   Exercise the aggregation result enterprise UI capability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_result.h"
#include <stdio.h>
int main(void){UmiUiEntAggregationResult r;umi_ui_ent_aggregation_result_init(&r,UMI_UI_ENT_AGG_AVERAGE);umi_ui_ent_aggregation_result_add(&r,2.0);umi_ui_ent_aggregation_result_add(&r,4.0);if(umi_ui_ent_aggregation_result_value(&r)!=3.0)return 1;puts("ok");return 0;}
