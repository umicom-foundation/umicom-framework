/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_load.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime load.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

void test_runtime_load(void)
{
    UmiProductExecutionRuntime r; UmiProductisationCompletionPlan p=test_plan_two_stage(); umi_product_execution_runtime_init(&r); assert(umi_product_execution_runtime_load_plan(&r,&p)==UMI_STATUS_OK); assert(r.loaded);
}
