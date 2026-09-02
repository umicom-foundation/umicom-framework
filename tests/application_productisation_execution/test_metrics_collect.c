/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_metrics_collect.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: metrics collect.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

/*
 * Exercise test metrics collect and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_metrics_collect(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; { UmiProductExecutionMetrics m=umi_product_execution_metrics_collect(&q); assert(m.total==2U); assert(m.succeeded==1U); assert(m.completion_percent==50U); }
}
