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

/* BEGINNER NOTE:
 * Chart data is independent of any frontend toolkit or renderer. Frontends render this shared model in their own way.
 */

#ifndef UMICOM_CHART_VIEWPORT_H
#define UMICOM_CHART_VIEWPORT_H
#include "umicom/chart/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiChartViewport { int64_t start_ms,end_ms; double min_value,max_value; int auto_scale; } UmiChartViewport;
void umi_chart_viewport_init(UmiChartViewport *viewport);
UmiStatus umi_chart_viewport_set_time(UmiChartViewport *viewport,int64_t start_ms,int64_t end_ms);
UmiStatus umi_chart_viewport_set_value(UmiChartViewport *viewport,double min_value,double max_value);
#ifdef __cplusplus
}
#endif
#endif
