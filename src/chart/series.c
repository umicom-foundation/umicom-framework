/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/series.c
 *
 * PURPOSE:
 *   Manage bounded chart point series.
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

#include "umicom/chart/series.h"
#include "../base/value_construction_internal.h"
#include "umicom/web/types.h"
#include <string.h>
/*
 * Initialise chart series from caller-provided values so later operations receive a known
 * state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_chart_series_init(UmiChartSeries *s,const char *id,UmiChartSeriesKind kind){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->kind=kind;return umi_web_copy_text(s->id,sizeof(s->id),id);}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_chart_series_init_build_value(UmiChartSeries *s,const char *id,UmiChartSeriesKind kind){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->kind=kind;return umi_web_copy_text(s->id,sizeof(s->id),id);}

UMI_DEFINE_STAGED_VALUE_INIT(umi_chart_series_init, UmiChartSeries, s,
    (UmiChartSeries *s, const char *id, UmiChartSeriesKind kind),
    umi_chart_series_init_build_value(candidate, id, kind))

/* Add chart series only after its inputs and available capacity have been checked. */
UmiStatus umi_chart_series_add(UmiChartSeries *s,UmiChartPoint p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->point_count>=UMI_CHART_MAX_POINTS)return UMI_STATUS_CAPACITY_EXCEEDED;s->points[s->point_count++]=p;return UMI_STATUS_OK;}
/* Release or reset state held by chart series so the same storage can be reused safely. */
void umi_chart_series_clear(UmiChartSeries *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)s->point_count=0U;}
