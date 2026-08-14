/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/execution_adapter_v2.c
 * PURPOSE: Dispatch approved Helix actions to the correct host callback.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/execution_adapter_v2.h"

UmiStatus umi_helix_execution_adapter_v2_validate(
    const UmiHelixExecutionAdapterV2 *adapter)
{
    if (adapter == NULL || adapter->read == NULL || adapter->filesystem == NULL ||
        adapter->build == NULL || adapter->test == NULL ||
        adapter->review == NULL || adapter->source_control == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_execution_adapter_v2_execute(
    const UmiHelixExecutionAdapterV2 *adapter,
    const UmiHelixActionV2 *action,
    int approved,
    char *out_evidence,
    size_t evidence_capacity)
{
    UmiHelixExecuteActionV2 callback = NULL;
    UmiStatus status = umi_helix_execution_adapter_v2_validate(adapter);
    if (status != UMI_STATUS_OK || action == NULL || out_evidence == NULL ||
        evidence_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_helix_action_v2_requires_approval(action) && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    switch (action->kind) {
        case UMI_HELIX_ACTION_READ: callback = adapter->read; break;
        case UMI_HELIX_ACTION_FILESYSTEM: callback = adapter->filesystem; break;
        case UMI_HELIX_ACTION_BUILD: callback = adapter->build; break;
        case UMI_HELIX_ACTION_TEST: callback = adapter->test; break;
        case UMI_HELIX_ACTION_REVIEW: callback = adapter->review; break;
        case UMI_HELIX_ACTION_SOURCE_CONTROL:
            callback = adapter->source_control;
            break;
        default: return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_evidence[0] = '\0';
    return callback(adapter->context, action, out_evidence, evidence_capacity);
}
