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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent aggregation result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntAggregationResult { UmiUiEntAggregateKind kind; size_t count; double sum; double minimum; double maximum; int initialized; } UmiUiEntAggregationResult;
/**
 * Initialise ui ent aggregation result from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_aggregation_result_init(UmiUiEntAggregationResult *r,UmiUiEntAggregateKind kind);
/**
 * Add ui ent aggregation result only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_aggregation_result_add(UmiUiEntAggregationResult *r,double value);
/**
 * Provide the ui ent aggregation result value operation used by this module and its client
 * applications.
 */
double umi_ui_ent_aggregation_result_value(const UmiUiEntAggregationResult *r);


#ifdef __cplusplus
}
#endif
#endif
