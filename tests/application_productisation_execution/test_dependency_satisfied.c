/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_dependency_satisfied.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: dependency satisfied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_dependency_satisfied(void)
{
    UmiProductExecutionWorkQueue q; UmiProductExecutionDependencyGraph g; UmiProductisationCompletionPlan p=test_plan_two_stage(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); umi_product_execution_dependency_graph_init(&g); assert(umi_product_execution_dependency_add(&g,0U,1U,2U)==UMI_STATUS_OK); assert(!umi_product_execution_dependencies_satisfied(&g,&q,1U)); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; assert(umi_product_execution_dependencies_satisfied(&g,&q,1U));
}
