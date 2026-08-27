/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_checkpoint_latest.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: checkpoint latest.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_checkpoint_latest(void)
{
    UmiProductExecutionCheckpointLedger l; UmiProductExecutionWorkQueue q; UmiProductisationCompletionPlan p=test_plan_two_stage(); umi_product_execution_checkpoint_ledger_init(&l); assert(umi_product_execution_work_queue_from_plan(&q,&p,3U)==UMI_STATUS_OK); assert(umi_product_execution_checkpoint_capture(&l,&q,"cp")==UMI_STATUS_OK); assert(umi_product_execution_checkpoint_latest(&l)!=NULL);
}
