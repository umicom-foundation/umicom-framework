/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/clean_machine.c
 *
 * PURPOSE:
 *   Implement evaluate whether installed runtime state is sufficient outside a source tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/clean_machine.h"
#include <string.h>

/*
 * Initialise test runtime clean machine from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_clean_machine_init(UmiTestRuntimeCleanMachine *value, const char *id)
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
 * Check that test runtime clean machine satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_clean_machine_validate(const UmiTestRuntimeCleanMachine *value)
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
 * Provide the test runtime clean machine set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_set_name(UmiTestRuntimeCleanMachine *value, const char *name)
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
 * Provide the test runtime clean machine set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_set_detail(UmiTestRuntimeCleanMachine *value, const char *detail)
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
 * Return the number of records represented by test runtime clean machine set requirement
 * without changing their state.
 */
UmiStatus umi_test_runtime_clean_machine_set_requirement_count(UmiTestRuntimeCleanMachine *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->requirement_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test runtime clean machine set missing
 * without changing their state.
 */
UmiStatus umi_test_runtime_clean_machine_set_missing_count(UmiTestRuntimeCleanMachine *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->missing_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime clean machine touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_clean_machine_touch(UmiTestRuntimeCleanMachine *value, uint64_t updated_at_ms)
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
 * Provide the test runtime clean machine same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_clean_machine_same_identity(const UmiTestRuntimeCleanMachine *left, const UmiTestRuntimeCleanMachine *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
