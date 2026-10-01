/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/variable_request.inc
 * PURPOSE: Bind one variables request to an unchanged inspected native stop and owned workspace.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "variable_inspection_private.h"
UmiStatus UmiDebugRuntimeInspectVariable(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target,
    uint32_t timeout_ms, UmiDebugVariablePage **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (platform == NULL || workspace == NULL || target == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiDebugWorkspaceVariableService(workspace) != platform->service) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugVariableTargetValidate(workspace, target);
    if (status == UMI_STATUS_OK) status = UmiDebugVariableTargetExpandable(target);
    if (status != UMI_STATUS_OK) return status;
    if (!platform->active || !platform->paused || !platform->initialized || platform->adapter == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK) return status;
    if (adapter.queued_events != 0U) return UMI_STATUS_BUSY;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED || platform->active_frame_id > INT32_MAX)
        return UMI_STATUS_INVALID_STATE;
    char frameId[32]; (void)snprintf(frameId, sizeof frameId, "%llu", (unsigned long long)platform->active_frame_id);
    if (strcmp(target->stamp.selectedFrame, frameId) != 0 || !target->thread.stopped ||
        target->thread.native_id != platform->active_thread_id ||
        strcmp(target->thread.session_id, platform->active_session_id) != 0) return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugVariableChildren *children = malloc(sizeof *children);
    if (response == NULL || children == NULL) { free(response); free(children); return UMI_STATUS_OUT_OF_MEMORY; }
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_variables(platform->adapter, target->value.variables_reference, &sequence);
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_wait_response(
        platform->adapter, sequence, timeout_ms, response);
    if (status == UMI_STATUS_OK && strcmp(response->command, "variables") != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiDebugRuntimeDecodeVariableChildren(response->json, children);
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    /* wait_response buffers events without applying them. A continued, stopped
     * or exited event makes these numeric references uncertain until processed. */
    if (status == UMI_STATUS_OK && adapter.queued_events != 0U) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) status = UmiDebugVariableTargetValidate(workspace, target);
    if (status == UMI_STATUS_OK) status = UmiDebugVariablePageCreate(target, children, out);
    free(response); free(children); return status;
}
