/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/crash_evidence.c
 *
 * PURPOSE:
 *   Implement retain crash status, fault address and failure category evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/crash_evidence.h"
#include <string.h>

/*
 * Initialise test runtime crash evidence from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_crash_evidence_init(UmiTestRuntimeCrashEvidence *value, const char *id)
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
 * Check that test runtime crash evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_crash_evidence_validate(const UmiTestRuntimeCrashEvidence *value)
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
 * Provide the test runtime crash evidence set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_name(UmiTestRuntimeCrashEvidence *value, const char *name)
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
 * Provide the test runtime crash evidence set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_detail(UmiTestRuntimeCrashEvidence *value, const char *detail)
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
 * Provide the test runtime crash evidence set native status operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_native_status(UmiTestRuntimeCrashEvidence *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->native_status = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime crash evidence set thread id operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_set_thread_id(UmiTestRuntimeCrashEvidence *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->thread_id = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime crash evidence touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_crash_evidence_touch(UmiTestRuntimeCrashEvidence *value, uint64_t updated_at_ms)
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
 * Provide the test runtime crash evidence same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_crash_evidence_same_identity(const UmiTestRuntimeCrashEvidence *left, const UmiTestRuntimeCrashEvidence *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
