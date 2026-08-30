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
void umi_analytics_pipeline_init(UmiAnalyticsPipeline *pipeline){if(pipeline!=NULL)memset(pipeline,0,sizeof *pipeline);}
UmiStatus umi_analytics_pipeline_add(UmiAnalyticsPipeline *pipeline,UmiAnalyticsStageKind stage){if(pipeline==NULL||stage<UMI_ANALYTICS_STAGE_FILTER||stage>UMI_ANALYTICS_STAGE_LIMIT)return UMI_STATUS_INVALID_ARGUMENT;if(pipeline->count>=16U)return UMI_STATUS_CAPACITY_EXCEEDED;pipeline->stages[pipeline->count++]=stage;return UMI_STATUS_OK;}
int umi_analytics_pipeline_valid(const UmiAnalyticsPipeline *pipeline){size_t i;int aggregate_seen=0;if(pipeline==NULL||pipeline->count==0U)return 0;for(i=0;i<pipeline->count;i++){if(pipeline->stages[i]==UMI_ANALYTICS_STAGE_AGGREGATE)aggregate_seen=1;if(pipeline->stages[i]==UMI_ANALYTICS_STAGE_GROUP&&aggregate_seen)return 0;}return 1;}
