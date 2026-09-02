/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_query.c
 *
 * PURPOSE:
 *   Describe provider-neutral analytical queries for dashboard datasets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_query.h"

#include <string.h>
/*
 * Initialise analytics query from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_query_init(UmiAnalyticsQuery *q,const char *dataset,const char *metric){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(q,0,sizeof *q);s=umi_analytics_copy_text(q->dataset_id,sizeof q->dataset_id,dataset);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;s=umi_analytics_copy_text(q->metric_id,sizeof q->metric_id,metric);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=0)return s;q->limit=1000U;return UMI_STATUS_OK;}
/* Check that analytics query satisfies its contract before another service relies on it. */
int umi_analytics_query_valid(const UmiAnalyticsQuery *q){return q!=NULL&&q->dataset_id[0]!='\0'&&q->metric_id[0]!='\0'&&q->start_ns<=q->end_ns&&q->limit>0U;}
