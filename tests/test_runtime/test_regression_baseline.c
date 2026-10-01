/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_regression_baseline.c
 *
 * PURPOSE:
 *   Verify the regression baseline contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/regression_baseline.h"

/* A refused edit must preserve the whole record, including its observation
 * token. Capture bytes from this same object so padding is compared only for
 * an operation that promises to perform no writes. */
static int CheckRefusedEdits(void)
{
    UmiTestRuntimeRegressionBaseline value;
    umi_test_runtime_regression_baseline_init(&value, "publication-check");
    unsigned char before[sizeof(value)];
    {
        char oversized[sizeof(value.detail) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_regression_baseline_set_detail(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_regression_baseline_set_detail(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_regression_baseline_set_detail(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_regression_baseline_set_detail(&value, value.detail + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.detail, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    {
        char oversized[sizeof(value.name) + 1U];
        memset(oversized, 'x', sizeof(oversized));
        oversized[sizeof(oversized) - 1U] = '\0';
        if (umi_test_runtime_regression_baseline_set_name(&value, "retained") != UMI_STATUS_OK) return 1;
        memcpy(before, &value, sizeof(value));
        if (umi_test_runtime_regression_baseline_set_name(&value, oversized) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        if (umi_test_runtime_regression_baseline_set_name(&value, NULL) != UMI_STATUS_INVALID_ARGUMENT) return 1;
        if (memcmp(before, &value, sizeof(value)) != 0) return 1;
        uint64_t observed = value.revision;
        if (umi_test_runtime_regression_baseline_set_name(&value, value.name + 1) != UMI_STATUS_OK) return 1;
        if (strcmp(value.name, "etained") != 0 || value.revision != observed + 1U) return 1;
    }
    /* The final valid edit may reach the boundary. Every later mutator must
     * refuse before changing any field, rather than wrapping the counter. */
    value.revision = UINT64_MAX - 1U;
    if (umi_test_runtime_regression_baseline_set_name(&value, "Regression Runtime") != UMI_STATUS_OK || value.revision != UINT64_MAX) return 1;
    memcpy(before, &value, sizeof(value));
    if (umi_test_runtime_regression_baseline_set_name(&value, "Regression Runtime") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_regression_baseline_set_detail(&value, "deterministic evidence") != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_regression_baseline_set_expected_passed(&value, 7U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_regression_baseline_set_expected_failed(&value, 11U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    if (umi_test_runtime_regression_baseline_touch(&value, 1234U) != UMI_STATUS_CAPACITY_EXCEEDED) return 1;
    if (memcmp(before, &value, sizeof(value)) != 0) return 1;
    return 0;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

int main(void)
{
    if (CheckRefusedEdits() != 0) return 1;
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
