/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/runtime_probe.c
 *
 * PURPOSE:
 *   Implement probe compiler runtime, build output and platform dependency directories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/runtime_probe.h"
#include <string.h>

/*
 * Initialise test runtime runtime probe from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_runtime_probe_init(UmiTestRuntimeRuntimeProbe *value, const char *id)
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
 * Check that test runtime runtime probe satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_runtime_probe_validate(const UmiTestRuntimeRuntimeProbe *value)
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
 * Provide the test runtime runtime probe set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_set_name(UmiTestRuntimeRuntimeProbe *value, const char *name)
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
 * Provide the test runtime runtime probe set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_set_detail(UmiTestRuntimeRuntimeProbe *value, const char *detail)
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
 * Return the number of records represented by test runtime runtime probe set probe without
 * changing their state.
 */
UmiStatus umi_test_runtime_runtime_probe_set_probe_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->probe_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test runtime runtime probe set failure
 * without changing their state.
 */
UmiStatus umi_test_runtime_runtime_probe_set_failure_count(UmiTestRuntimeRuntimeProbe *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->failure_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime runtime probe touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_runtime_probe_touch(UmiTestRuntimeRuntimeProbe *value, uint64_t updated_at_ms)
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
 * Provide the test runtime runtime probe same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_runtime_probe_same_identity(const UmiTestRuntimeRuntimeProbe *left, const UmiTestRuntimeRuntimeProbe *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
