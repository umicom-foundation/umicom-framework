/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_step.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime step.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_runtime_step(void)
{
    UmiProductExecutionRuntime r; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionAdapter a=test_adapter(); umi_product_execution_runtime_init(&r); assert(umi_product_execution_runtime_set_adapter(&r,&a)==UMI_STATUS_OK); assert(umi_product_execution_runtime_load_plan(&r,&p)==UMI_STATUS_OK); assert(umi_product_execution_runtime_step(&r)==UMI_STATUS_OK); assert(r.queue.items[0].state==UMI_PRODUCT_EXECUTION_SUCCEEDED);
}
