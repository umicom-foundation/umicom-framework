/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/snapshot.c
 *
 * PURPOSE:
 *   Expose a compact immutable chart-state snapshot.
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

#include "umicom/chart/snapshot.h"
#include <string.h>
UmiStatus umi_chart_snapshot(const UmiChartModel *m,UmiChartSnapshot *out){size_t i,total=0U;if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));(void)memcpy(out->title,m->title,sizeof(out->title));out->series_count=m->series_count;out->revision=m->revision;for(i=0U;i<m->series_count;++i)total+=m->series[i].point_count;out->point_count=total;return UMI_STATUS_OK;}
