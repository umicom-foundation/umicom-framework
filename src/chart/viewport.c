/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/viewport.c
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
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/viewport.h"
void umi_chart_viewport_init(UmiChartViewport *v){if(v==NULL)return;v->start_ms=0;v->end_ms=0;v->min_value=0.0;v->max_value=0.0;v->auto_scale=1;}
UmiStatus umi_chart_viewport_set_time(UmiChartViewport *v,int64_t a,int64_t b){if(v==NULL||b<a)return UMI_STATUS_INVALID_ARGUMENT;v->start_ms=a;v->end_ms=b;return UMI_STATUS_OK;}
UmiStatus umi_chart_viewport_set_value(UmiChartViewport *v,double a,double b){if(v==NULL||b<a)return UMI_STATUS_INVALID_ARGUMENT;v->min_value=a;v->max_value=b;v->auto_scale=0;return UMI_STATUS_OK;}
