/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_duration_budget.c
 *
 * PURPOSE:
 *   Verify the duration budget runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/duration_budget.h"
int main(void)
{
    UmiTestRuntimeDurationBudget value,same;
    uint64_t r;
    umi_test_runtime_duration_budget_init(&value,"test-runtime.duration_budget");
    assert(umi_test_runtime_duration_budget_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_duration_budget_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_duration_budget_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_duration_budget_set_expected_ms(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_duration_budget_set_maximum_ms(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_duration_budget_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.expected_ms==13U&&value.maximum_ms==21U);
    umi_test_runtime_duration_budget_init(&same,"test-runtime.duration_budget");
    assert(umi_test_runtime_duration_budget_same_identity(&value,&same));
    return 0;
    }
