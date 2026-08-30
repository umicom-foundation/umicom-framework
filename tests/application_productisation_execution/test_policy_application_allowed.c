/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_policy_application_allowed.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: policy application allowed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_policy_application_allowed(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionPolicy pol=umi_product_execution_policy_default(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; assert(umi_product_execution_policy_check_item(&pol,&q,1U)==UMI_STATUS_OK);
}
