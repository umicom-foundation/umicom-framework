/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_pipeline.c
 *
 * PURPOSE:
 *   Validate analytics_pipeline analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/analytics_pipeline.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiAnalyticsPipeline p;umi_analytics_pipeline_init(&p);umi_analytics_pipeline_add(&p,UMI_ANALYTICS_STAGE_FILTER);umi_analytics_pipeline_add(&p,UMI_ANALYTICS_STAGE_GROUP);umi_analytics_pipeline_add(&p,UMI_ANALYTICS_STAGE_AGGREGATE);return umi_analytics_pipeline_valid(&p)?0:1;}
