/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_dependency_stage_barrier.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: dependency stage barrier.
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
 * Exercise test dependency stage barrier and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_dependency_stage_barrier(void)
{
    UmiProductExecutionWorkQueue q; UmiProductExecutionDependencyGraph g; UmiProductisationCompletionPlan p=test_plan_two_stage(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); umi_product_execution_dependency_graph_init(&g); assert(umi_product_execution_dependency_graph_add_stage_barriers(&g,&q)==UMI_STATUS_OK); assert(g.count==1U);
}
