/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/aggregation_descriptor.c
 *
 * PURPOSE:
 *   Describe one summary aggregation over a column.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/aggregation_descriptor.h"
#include <string.h>
UmiStatus umi_ui_ent_aggregation_descriptor_init(UmiUiEntAggregationDescriptor *value){if(!value)return UMI_STATUS_INVALID_ARGUMENT;memset(value,0,sizeof *value);value->aggregation_id[0]='\0';value->column_id[0]='\0';value->kind=0;return UMI_STATUS_OK;}
int umi_ui_ent_aggregation_descriptor_validate(const UmiUiEntAggregationDescriptor *value){return value!=NULL&&umi_ui_ent_id_valid(value->aggregation_id)&&umi_ui_ent_id_valid(value->column_id)&&value->kind>=UMI_UI_ENT_AGG_COUNT&&value->kind<=UMI_UI_ENT_AGG_AVERAGE;}
