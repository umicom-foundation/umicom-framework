/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_parallel_budget.c
 *
 * PURPOSE:
 *   Verify the parallel budget runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/parallel_budget.h"
int main(void)
{
    UmiTestRuntimeParallelBudget value,same;
    uint64_t r;
    umi_test_runtime_parallel_budget_init(&value,"test-runtime.parallel_budget");
    assert(umi_test_runtime_parallel_budget_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_parallel_budget_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_parallel_budget_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_parallel_budget_set_maximum_parallel(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_parallel_budget_set_processor_cost(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_parallel_budget_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.maximum_parallel==13U&&value.processor_cost==21U);
    umi_test_runtime_parallel_budget_init(&same,"test-runtime.parallel_budget");
    assert(umi_test_runtime_parallel_budget_same_identity(&value,&same));
    return 0;
    }
