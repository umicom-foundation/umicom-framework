/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/sdk_runtime/dependency_evidence.c
 *
 * PURPOSE:
 *   Implement retain resolved dependency path and provenance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/sdk_runtime/dependency_evidence.h"
#include <string.h>
/*
 * Initialise sdk runtime dependency evidence from caller-provided values so later
 * operations receive a known state.
 */
void umi_sdk_runtime_dependency_evidence_init(UmiSdkRuntimeDependencyEvidence *value, const char *id)
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
 * Check that sdk runtime dependency evidence satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_validate(const UmiSdkRuntimeDependencyEvidence *value)
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
 * Provide the sdk runtime dependency evidence set path operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_set_path(UmiSdkRuntimeDependencyEvidence *value, const char *path)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->path, sizeof(value->path), path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime dependency evidence set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_set_detail(UmiSdkRuntimeDependencyEvidence *value, const char *detail)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_sdk_runtime_copy_text(value->detail, sizeof(value->detail), detail); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) value->revision += 1U; return status;
}
/*
 * Provide the sdk runtime dependency evidence set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_set_required(UmiSdkRuntimeDependencyEvidence *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->required = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime dependency evidence set resolved operation used by this module
 * and its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_set_resolved(UmiSdkRuntimeDependencyEvidence *value, uint64_t number)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT; value->resolved = number; value->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the sdk runtime dependency evidence set state operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_dependency_evidence_set_state(UmiSdkRuntimeDependencyEvidence *value, UmiSdkRuntimeState state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || state > UMI_SDK_RUNTIME_STATE_MISSING) return UMI_STATUS_INVALID_ARGUMENT;
    value->state = state; value->revision += 1U; return UMI_STATUS_OK;
}
/*
 * Provide the sdk runtime dependency evidence same identity operation used by this module
 * and its client applications.
 */
bool umi_sdk_runtime_dependency_evidence_same_identity(const UmiSdkRuntimeDependencyEvidence *left, const UmiSdkRuntimeDependencyEvidence *right)
{ return left != NULL && right != NULL && strcmp(left->id, right->id) == 0; }
