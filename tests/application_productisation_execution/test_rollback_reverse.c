/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_rollback_reverse.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: rollback reverse.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_rollback_reverse(void)
{
    UmiProductExecutionWorkQueue q; UmiProductExecutionRollbackPlan r; UmiProductisationCompletionPlan p=test_plan_two_stage(); size_t idx; assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; q.items[1].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; assert(umi_product_execution_rollback_plan_build(&r,&q)==UMI_STATUS_OK); assert(umi_product_execution_rollback_next(&r,&idx)==UMI_STATUS_OK); assert(idx==1U);
}
