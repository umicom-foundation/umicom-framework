/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation_execution/test_runtime_snapshot.c
 *
 * PURPOSE:
 *   Validate productisation execution behaviour: runtime snapshot.
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
 * Exercise test runtime snapshot and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_runtime_snapshot(void)
{
    UmiProductExecutionRuntime r; UmiProductisationCompletionPlan p=test_plan_two_stage(); umi_product_execution_runtime_init(&r); assert(umi_product_execution_runtime_load_plan(&r,&p)==UMI_STATUS_OK); { UmiProductExecutionSnapshot s=umi_product_execution_runtime_snapshot(&r); assert(s.metrics.total==2U); assert(s.revision==r.revision); }
}
