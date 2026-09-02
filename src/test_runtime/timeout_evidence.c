/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/timeout_evidence.c
 *
 * PURPOSE:
 *   Implement retain timeout budgets, elapsed time and cancellation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/timeout_evidence.h"
#include <string.h>

/*
 * Initialise test runtime timeout evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_timeout_evidence_init(UmiTestRuntimeTimeoutEvidence *value, const char *id)
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
 * Check that test runtime timeout evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_timeout_evidence_validate(const UmiTestRuntimeTimeoutEvidence *value)
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
 * Provide the test runtime timeout evidence set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_name(UmiTestRuntimeTimeoutEvidence *value, const char *name)
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
 * Provide the test runtime timeout evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_detail(UmiTestRuntimeTimeoutEvidence *value, const char *detail)
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
 * Provide the test runtime timeout evidence set elapsed ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_elapsed_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->elapsed_ms = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime timeout evidence set budget ms operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_set_budget_ms(UmiTestRuntimeTimeoutEvidence *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->budget_ms = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime timeout evidence touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_timeout_evidence_touch(UmiTestRuntimeTimeoutEvidence *value, uint64_t updated_at_ms)
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
 * Provide the test runtime timeout evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_timeout_evidence_same_identity(const UmiTestRuntimeTimeoutEvidence *left, const UmiTestRuntimeTimeoutEvidence *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
