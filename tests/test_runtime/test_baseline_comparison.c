/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_baseline_comparison.c
 *
 * PURPOSE:
 *   Verify the baseline comparison runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/baseline_comparison.h"
int main(void)
{
    UmiTestRuntimeBaselineComparison value,same;
    uint64_t r;
    umi_test_runtime_baseline_comparison_init(&value,"test-runtime.baseline_comparison");
    assert(umi_test_runtime_baseline_comparison_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_baseline_comparison_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_baseline_comparison_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_baseline_comparison_set_new_failures(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_baseline_comparison_set_resolved_failures(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_baseline_comparison_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.new_failures==13U&&value.resolved_failures==21U);
    umi_test_runtime_baseline_comparison_init(&same,"test-runtime.baseline_comparison");
    assert(umi_test_runtime_baseline_comparison_same_identity(&value,&same));
    return 0;
    }
