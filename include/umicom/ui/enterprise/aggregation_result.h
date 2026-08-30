/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/aggregation_result.h
 *
 * PURPOSE:
 *   Accumulate count/sum/min/max/average results over numeric virtual cells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_AGGREGATION_RESULT_H
#define UMICOM_UI_ENTERPRISE_AGGREGATION_RESULT_H

#include "umicom/ui/enterprise/types.h"
typedef struct UmiUiEntAggregationResult { UmiUiEntAggregateKind kind; size_t count; double sum; double minimum; double maximum; int initialized; } UmiUiEntAggregationResult;
void umi_ui_ent_aggregation_result_init(UmiUiEntAggregationResult *r,UmiUiEntAggregateKind kind);
UmiStatus umi_ui_ent_aggregation_result_add(UmiUiEntAggregationResult *r,double value);
double umi_ui_ent_aggregation_result_value(const UmiUiEntAggregationResult *r);

#endif
