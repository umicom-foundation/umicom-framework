/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/json.c
 *
 * PURPOSE:
 *   Serialise chart state for browser and remote frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The calculation is deliberately straightforward and deterministic. Frontends can optimise rendering without changing these shared data contracts.
 */

#include "umicom/chart/json.h"
#include "umicom/web/json.h"
#include <stdio.h>
UmiStatus umi_chart_model_to_json(const UmiChartModel *m,char *out,size_t cap){char title[UMI_CHART_NAME_CAPACITY*2U];int n;if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(umi_web_json_escape(m->title,title,sizeof(title))!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;n=snprintf(out,cap,"{\"title\":\"%s\",\"series\":%zu,\"revision\":%llu}",title,m->series_count,(unsigned long long)m->revision);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
