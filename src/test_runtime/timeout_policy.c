/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/timeout_policy.c
 *
 * PURPOSE:
 *   Implement define bounded start, execution and shutdown time budgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/timeout_policy.h"
#include <string.h>

/*
 * Initialise test runtime timeout policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_timeout_policy_init(UmiTestRuntimeTimeoutPolicy *value, const char *id)
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
 * Check that test runtime timeout policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_timeout_policy_validate(const UmiTestRuntimeTimeoutPolicy *value)
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
 * Provide the test runtime timeout policy set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_name(UmiTestRuntimeTimeoutPolicy *value, const char *name)
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
 * Provide the test runtime timeout policy set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_detail(UmiTestRuntimeTimeoutPolicy *value, const char *detail)
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
 * Provide the test runtime timeout policy set start timeout ms operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_start_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->start_timeout_ms = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime timeout policy set execution timeout ms operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_set_execution_timeout_ms(UmiTestRuntimeTimeoutPolicy *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->execution_timeout_ms = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime timeout policy touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_policy_touch(UmiTestRuntimeTimeoutPolicy *value, uint64_t updated_at_ms)
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
 * Provide the test runtime timeout policy same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_timeout_policy_same_identity(const UmiTestRuntimeTimeoutPolicy *left, const UmiTestRuntimeTimeoutPolicy *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
