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
int main(void){UmiUiEntAggregationDescriptor v;if(umi_ui_ent_aggregation_descriptor_init(&v)!=UMI_STATUS_OK)return 1;if(umi_ui_ent_copy_text(v.aggregation_id,sizeof v.aggregation_id,"id")!=UMI_STATUS_OK)return 2;if(umi_ui_ent_copy_text(v.column_id,sizeof v.column_id,"value")!=UMI_STATUS_OK)return 3;v.kind=UMI_UI_ENT_AGG_SUM;if(!umi_ui_ent_aggregation_descriptor_validate(&v))return 9;puts("ok");return 0;}
