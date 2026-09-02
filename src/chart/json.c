/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/json.c
 *
 * PURPOSE:
 *   Serialise chart state for browser and remote frontends.
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

#include "umicom/chart/json.h"
#include "umicom/web/json.h"
#include <stdio.h>
/*
 * Provide the chart model to json operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_model_to_json(const UmiChartModel *m,char *out,size_t cap){char title[UMI_CHART_NAME_CAPACITY*2U];int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_web_json_escape(m->title,title,sizeof(title))!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;n=snprintf(out,cap,"{\"title\":\"%s\",\"series\":%zu,\"revision\":%llu}",title,m->series_count,(unsigned long long)m->revision);return n<0||(size_t)n>=cap?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_OK;}
