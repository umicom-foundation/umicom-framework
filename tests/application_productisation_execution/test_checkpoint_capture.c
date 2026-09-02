/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_checkpoint_capture.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: checkpoint capture.
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
 * Exercise test checkpoint capture and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_checkpoint_capture(void)
{
    UmiProductExecutionCheckpointLedger l; UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); umi_product_execution_checkpoint_ledger_init(&l); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); q.items[0].state=UMI_PRODUCT_EXECUTION_SUCCEEDED; assert(umi_product_execution_checkpoint_capture(&l,&q,"cp")==UMI_STATUS_OK); assert(l.checkpoints[0].succeeded==1U);
}
