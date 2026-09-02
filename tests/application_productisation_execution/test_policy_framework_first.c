/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_policy_framework_first.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: policy framework first.
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
 * Exercise test policy framework first and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_policy_framework_first(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionPolicy pol=umi_product_execution_policy_default(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); assert(umi_product_execution_policy_check_item(&pol,&q,1U)==UMI_STATUS_BUSY);
}
