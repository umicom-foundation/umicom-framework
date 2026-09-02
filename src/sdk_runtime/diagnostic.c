/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/diagnostic.c
 *
 * PURPOSE:
 *   Implement represent actionable sdk/runtime diagnostic evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/diagnostic.h"
#include <string.h>
/*
 * Initialise sdk runtime diagnostic from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_diagnostic_init(UmiSdkRuntimeDiagnostic *value, const char *id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->structure_size = (uint32_t)sizeof(*value);
    value->state = UMI_SDK_RUNTIME_STATE_UNKNOWN;
    value->enabled = true;
    value->revision = 1U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (id != NULL) (void)umi_sdk_runtime_copy_text(value->id, sizeof(value->id), id);
}
/*
 * Check that sdk runtime diagnostic satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_sdk_runtime_diagnostic_validate(const UmiSdkRuntimeDiagnostic *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->structure_size != sizeof(*value)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->id, sizeof(value->id)) || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->path, sizeof(value->path))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_sdk_runtime_text_is_valid(value->detail, sizeof(value->detail))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (value->state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime diagnostic set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_path(UmiSdkRuntimeDiagnostic *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime diagnostic set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_detail(UmiSdkRuntimeDiagnostic *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime diagnostic set severity operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_severity(UmiSdkRuntimeDiagnostic *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->severity = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime diagnostic set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_sequence(UmiSdkRuntimeDiagnostic *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->sequence = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime diagnostic set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_diagnostic_set_state(UmiSdkRuntimeDiagnostic *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime diagnostic same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_diagnostic_same_identity(const UmiSdkRuntimeDiagnostic *left, const UmiSdkRuntimeDiagnostic *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
