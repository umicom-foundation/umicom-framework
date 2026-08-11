/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/model.c
 *
 * PURPOSE:
 *   Own the toolkit-neutral chart model and series catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/model.h"
#include "umicom/web/types.h"
#include <string.h>
UmiStatus umi_chart_model_init(UmiChartModel *m,const char *title){if(m==NULL||title==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(m,0,sizeof(*m));umi_chart_viewport_init(&m->viewport);return umi_web_copy_text(m->title,sizeof(m->title),title);}
UmiStatus umi_chart_model_add_series(UmiChartModel *m,const UmiChartSeries *s){if(m==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(m->series_count>=UMI_CHART_MAX_SERIES)return UMI_STATUS_CAPACITY_EXCEEDED;m->series[m->series_count++]=*s;++m->revision;return UMI_STATUS_OK;}
UmiChartSeries *umi_chart_model_find_series(UmiChartModel *m,const char *id){size_t i;if(m==NULL||id==NULL)return NULL;for(i=0U;i<m->series_count;++i)if(strcmp(m->series[i].id,id)==0)return &m->series[i];return NULL;}
