/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_checkpoint.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime checkpoint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_runtime_checkpoint(void)
{
    UmiProductExecutionRuntime r; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionAdapter a=test_adapter(); umi_product_execution_runtime_init(&r); assert(umi_product_execution_runtime_set_adapter(&r,&a)==UMI_STATUS_OK); assert(umi_product_execution_runtime_load_plan(&r,&p)==UMI_STATUS_OK); assert(umi_product_execution_runtime_checkpoint(&r,"start")==UMI_STATUS_OK); assert(r.checkpoints.count==1U);
}
