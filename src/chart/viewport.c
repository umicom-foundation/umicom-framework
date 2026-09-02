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
/*
 * Initialise chart viewport from caller-provided values so later operations receive a
 * known state.
 */
void umi_chart_viewport_init(UmiChartViewport *v){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL)return;v->start_ms=0;v->end_ms=0;v->min_value=0.0;v->max_value=0.0;v->auto_scale=1;}
/*
 * Provide the chart viewport set time operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_viewport_set_time(UmiChartViewport *v,int64_t a,int64_t b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||b<a)return UMI_STATUS_INVALID_ARGUMENT;v->start_ms=a;v->end_ms=b;return UMI_STATUS_OK;}
/*
 * Provide the chart viewport set value operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_viewport_set_value(UmiChartViewport *v,double a,double b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v==NULL||b<a)return UMI_STATUS_INVALID_ARGUMENT;v->min_value=a;v->max_value=b;v->auto_scale=0;return UMI_STATUS_OK;}
