/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_framework_then_app.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime framework then app.
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
 * Exercise test runtime framework then app and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_runtime_framework_then_app(void)
{
    UmiProductExecutionRuntime r; UmiProductisationCompletionPlan p=test_plan_two_stage(); UmiProductExecutionAdapter a=test_adapter(); umi_product_execution_runtime_init(&r); assert(umi_product_execution_runtime_set_adapter(&r,&a)==UMI_STATUS_OK); assert(umi_product_execution_runtime_load_plan(&r,&p)==UMI_STATUS_OK); assert(umi_product_execution_runtime_step(&r)==UMI_STATUS_OK); assert(umi_product_execution_runtime_step(&r)==UMI_STATUS_OK); assert(r.queue.items[1].state==UMI_PRODUCT_EXECUTION_SUCCEEDED);
}
