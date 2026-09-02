/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/diagnostics.c
 *
 * PURPOSE:
 *   Implement maintain a bounded collection of runtime diagnostics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/diagnostics.h"
#include <string.h>

/*
 * Initialise test runtime diagnostics from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_diagnostics_init(UmiTestRuntimeDiagnostics *value, const char *id)
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
 * Check that test runtime diagnostics satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_diagnostics_validate(const UmiTestRuntimeDiagnostics *value)
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
 * Provide the test runtime diagnostics set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_name(UmiTestRuntimeDiagnostics *value, const char *name)
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
 * Provide the test runtime diagnostics set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_detail(UmiTestRuntimeDiagnostics *value, const char *detail)
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
 * Return the number of records represented by test runtime diagnostics set diagnostic
 * without changing their state.
 */
UmiStatus umi_test_runtime_diagnostics_set_diagnostic_count(UmiTestRuntimeDiagnostics *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->diagnostic_count = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime diagnostics set generation operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_diagnostics_set_generation(UmiTestRuntimeDiagnostics *value, uint64_t number)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    value->generation = number;
    value->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the test runtime diagnostics touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_diagnostics_touch(UmiTestRuntimeDiagnostics *value, uint64_t updated_at_ms)
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
 * Provide the test runtime diagnostics same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_diagnostics_same_identity(const UmiTestRuntimeDiagnostics *left, const UmiTestRuntimeDiagnostics *right)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return false;
    return strcmp(left->id, right->id) == 0;
}
