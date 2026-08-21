/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_regression_baseline.c
 *
 * PURPOSE:
 *   Verify the regression baseline contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/regression_baseline.h"

int main(void)
{
    UmiTestRuntimeRegressionBaseline value;
    UmiTestRuntimeRegressionBaseline same;
    uint64_t revision;
    umi_test_runtime_regression_baseline_init(&value, "test-runtime.regression_baseline");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_regression_baseline_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_regression_baseline_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_regression_baseline_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_regression_baseline_set_expected_passed(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_regression_baseline_set_expected_failed(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_regression_baseline_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.expected_passed == 7U);
    assert(value.expected_failed == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_regression_baseline_init(&same, "test-runtime.regression_baseline");
    assert(umi_test_runtime_regression_baseline_same_identity(&value, &same));
    assert(umi_test_runtime_regression_baseline_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
