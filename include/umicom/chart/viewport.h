/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/viewport.h
 *
 * PURPOSE:
 *   Represent visible chart time and value ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Chart data is independent of any frontend toolkit or renderer. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_VIEWPORT_H
#define UMICOM_CHART_VIEWPORT_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the chart viewport data shared with callers of this public contract.
 */
typedef struct UmiChartViewport { int64_t start_ms,end_ms; double min_value,max_value; int auto_scale; } UmiChartViewport;
/**
 * Initialise chart viewport from caller-provided values so later operations receive a
 * known state.
 */
void umi_chart_viewport_init(UmiChartViewport *viewport);
/**
 * Provide the chart viewport set time operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_viewport_set_time(UmiChartViewport *viewport,int64_t start_ms,int64_t end_ms);
/**
 * Provide the chart viewport set value operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_viewport_set_value(UmiChartViewport *viewport,double min_value,double max_value);
#ifdef __cplusplus
}
#endif
#endif
