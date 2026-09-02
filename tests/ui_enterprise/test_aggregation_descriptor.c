/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_aggregation_descriptor.c
 *
 * PURPOSE:
 *   Exercise the aggregation descriptor enterprise UI capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_descriptor.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiUiEntAggregationDescriptor v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_aggregation_descriptor_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.aggregation_id,sizeof v.aggregation_id,"id")!=UMI_STATUS_OK)return 2;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"value")!=UMI_STATUS_OK)return 3;v.kind=UMI_UI_ENT_AGG_SUM;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_aggregation_descriptor_validate(&v))return 9;puts("ok");return 0;}
