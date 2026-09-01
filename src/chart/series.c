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
#include "umicom/web/types.h"
#include <string.h>
UmiStatus umi_chart_series_init(UmiChartSeries *s,const char *id,UmiChartSeriesKind kind){if(s==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->kind=kind;return umi_web_copy_text(s->id,sizeof(s->id),id);}
UmiStatus umi_chart_series_add(UmiChartSeries *s,UmiChartPoint p){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(s->point_count>=UMI_CHART_MAX_POINTS)return UMI_STATUS_CAPACITY_EXCEEDED;s->points[s->point_count++]=p;return UMI_STATUS_OK;}
void umi_chart_series_clear(UmiChartSeries *s){if(s!=NULL)s->point_count=0U;}
