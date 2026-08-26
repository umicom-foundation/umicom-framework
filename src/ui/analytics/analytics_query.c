/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_query.c
 *
 * PURPOSE:
 *   Describe provider-neutral analytical queries for dashboard datasets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_query.h"

#include <string.h>
UmiStatus umi_analytics_query_init(UmiAnalyticsQuery *q,const char *dataset,const char *metric){UmiStatus s;if(q==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(q,0,sizeof *q);s=umi_analytics_copy_text(q->dataset_id,sizeof q->dataset_id,dataset);if(s!=0)return s;s=umi_analytics_copy_text(q->metric_id,sizeof q->metric_id,metric);if(s!=0)return s;q->limit=1000U;return UMI_STATUS_OK;}
int umi_analytics_query_valid(const UmiAnalyticsQuery *q){return q!=NULL&&q->dataset_id[0]!='\0'&&q->metric_id[0]!='\0'&&q->start_ns<=q->end_ns&&q->limit>0U;}
