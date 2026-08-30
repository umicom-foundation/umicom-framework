/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_acceptance_pending.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: acceptance pending.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_acceptance_pending(void)
{
    UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); assert(!umi_product_execution_acceptance_evaluate(&q).accepted);
}
