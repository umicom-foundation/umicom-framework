/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_executor_dry_run.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: executor dry run.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_executor_dry_run(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionPolicy pol=umi_product_execution_policy_default(); UmiProductExecutionAdapter a=test_adapter(); UmiProductExecutionCancellation c; UmiProductExecutionHistory h; pol.dry_run=1; assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_READY; umi_product_execution_cancellation_init(&c); umi_product_execution_history_init(&h); assert(umi_product_execution_execute_item(&q,0U,&pol,&a,&c,&h)==UMI_STATUS_OK); assert(strncmp(q.items[0].evidence_reference,"dry-run:",8)==0);
}
