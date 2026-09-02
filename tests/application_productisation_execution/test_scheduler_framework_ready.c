/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_scheduler_framework_ready.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: scheduler framework ready.
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
 * Exercise test scheduler framework ready and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_scheduler_framework_ready(void)
{
    UmiProductExecutionWorkQueue q; UmiProductExecutionDependencyGraph g; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionPolicy pol=umi_product_execution_policy_default(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); umi_product_execution_dependency_graph_init(&g); assert(umi_product_execution_dependency_graph_add_stage_barriers(&g,&q)==UMI_STATUS_OK); assert(umi_product_execution_scheduler_refresh_ready(&q,&g,&pol)==UMI_STATUS_OK); assert(q.items[0].state==UMI_PRODUCT_EXECUTION_READY); assert(q.items[1].state==UMI_PRODUCT_EXECUTION_PENDING);
}
