/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_acceptance_success.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: acceptance success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_acceptance_success(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; q.items[1].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; assert(umi_product_execution_acceptance_evaluate(&q).accepted);
}
