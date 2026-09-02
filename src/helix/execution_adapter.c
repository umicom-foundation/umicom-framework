/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/execution_adapter.c
 * PURPOSE: Dispatch approved Helix actions to the correct host callback.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/execution_adapter.h"

/*
 * Check that helix execution adapter satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_helix_execution_adapter_validate(
    const UmiHelixExecutionAdapter *adapter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->read == NULL || adapter->filesystem == NULL ||
        adapter->build == NULL || adapter->test == NULL ||
        adapter->review == NULL || adapter->source_control == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Perform helix execution adapter through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_helix_execution_adapter_execute(
    const UmiHelixExecutionAdapter *adapter,
    const UmiHelixAction *action,
    int approved,
    char *out_evidence,
    size_t evidence_capacity)
{
    UmiHelixExecuteAction callback = NULL;
    UmiStatus status = umi_helix_execution_adapter_validate(adapter);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || action == NULL || out_evidence == NULL ||
        evidence_capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_helix_action_requires_approval(action) && !approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Select the behaviour associated with the requested command or state value. */
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
