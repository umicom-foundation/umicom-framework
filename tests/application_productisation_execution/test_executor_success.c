/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_executor_success.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: executor success.
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
 * Exercise test executor success and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_executor_success(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionPolicy pol=umi_product_execution_policy_default(); UmiProductExecutionAdapter a=test_adapter(); UmiProductExecutionCancellation c; UmiProductExecutionHistory h; assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_READY; umi_product_execution_cancellation_init(&c); umi_product_execution_history_init(&h); assert(umi_product_execution_execute_item(&q,0U,&pol,&a,&c,&h)==UMI_STATUS_OK); assert(q.items[0].state==UMI_PRODUCT_EXECUTION_SUCCEEDED);
}
