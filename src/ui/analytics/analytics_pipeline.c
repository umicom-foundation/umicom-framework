/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/analytics_pipeline.c
 *
 * PURPOSE:
 *   Describe ordered analytical processing stages with structural validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_pipeline.h"

#include <string.h>
/*
 * Initialise analytics pipeline from caller-provided values so later operations receive a
 * known state.
 */
void umi_analytics_pipeline_init(UmiAnalyticsPipeline *pipeline){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline!=NULL)memset(pipeline,0,sizeof *pipeline);}
/* Add analytics pipeline only after its inputs and available capacity have been checked. */
UmiStatus umi_analytics_pipeline_add(UmiAnalyticsPipeline *pipeline,UmiAnalyticsStageKind stage){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline==NULL||stage<UMI_ANALYTICS_STAGE_FILTER||stage>UMI_ANALYTICS_STAGE_LIMIT)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline->count>=16U)return UMI_STATUS_CAPACITY_EXCEEDED;pipeline->stages[pipeline->count++]=stage;return UMI_STATUS_OK;}
/*
 * Check that analytics pipeline satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_pipeline_valid(const UmiAnalyticsPipeline *pipeline){size_t i;int aggregate_seen=0;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline==NULL||pipeline->count==0U)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<pipeline->count;i++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline->stages[i]==UMI_ANALYTICS_STAGE_AGGREGATE)aggregate_seen=1;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(pipeline->stages[i]==UMI_ANALYTICS_STAGE_GROUP&&aggregate_seen)return 0;}return 1;}
