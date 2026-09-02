/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/regression_baseline.c
 *
 * PURPOSE:
 *   Implement describe the expected pass/fail/timeout/crash baseline for comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/regression_baseline.h"
#include <string.h>

/*
 * Initialise test runtime regression baseline from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_regression_baseline_init(UmiTestRuntimeRegressionBaseline *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_test_runtime_copy_text(value->id, sizeof(value->id), id);
}

/*
 * Check that test runtime regression baseline satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_regression_baseline_validate(const UmiTestRuntimeRegressionBaseline *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->name, sizeof(value->name))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_test_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime regression baseline set name operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_regression_baseline_set_name(UmiTestRuntimeRegressionBaseline *value, const char *name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->name, sizeof(value->name), name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the test runtime regression baseline set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_regression_baseline_set_detail(UmiTestRuntimeRegressionBaseline *value, const char *detail)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_runtime_copy_text(value->detail, sizeof(value->detail), detail);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) value->revision += 1U;
    return status;
}

/*
 * Provide the test runtime regression baseline set expected passed operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_regression_baseline_set_expected_passed(UmiTestRuntimeRegressionBaseline *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->expected_passed = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime regression baseline set expected failed operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_regression_baseline_set_expected_failed(UmiTestRuntimeRegressionBaseline *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->expected_failed = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime regression baseline touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_regression_baseline_touch(UmiTestRuntimeRegressionBaseline *value, uint64_t updated_at_ms)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->updated_at_ms = updated_at_ms;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime regression baseline same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_regression_baseline_same_identity(const UmiTestRuntimeRegressionBaseline *left, const UmiTestRuntimeRegressionBaseline *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
