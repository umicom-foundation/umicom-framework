/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/memory_request.inc
 * PURPOSE: Tie each explicit memory request to an unchanged native stopped frame.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "memory_inspection_private.h"
UmiStatus UmiDebugRuntimeCheckMemory(UmiDebugRuntimePlatform *platform, UmiDebugWorkspace *workspace,
                                     const UmiDebugVariableTarget *target)
{
    if (platform == NULL || workspace == NULL || target == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiDebugWorkspaceVariableService(workspace) != platform->service)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugVariableTargetValidate(workspace, target);
    if (status != UMI_STATUS_OK)
        return status;
    if (!platform->active || !platform->paused || !platform->initialized || platform->adapter == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK)
        return status;
    if (adapter.queued_events != 0U)
        return UMI_STATUS_BUSY;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED || platform->active_frame_id > INT32_MAX)
        return UMI_STATUS_INVALID_STATE;
    char frameId[32];
    (void)snprintf(frameId, sizeof frameId, "%llu", (unsigned long long)platform->active_frame_id);
    if (strcmp(target->stamp.selectedFrame, frameId) != 0 || !target->thread.stopped ||
        target->thread.native_id != platform->active_thread_id ||
        strcmp(target->thread.session_id, platform->active_session_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_read_memory_request)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (target->value.memory_reference[0] == '\0')
        return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugRuntimeInspectMemory(UmiDebugRuntimePlatform *platform, UmiDebugWorkspace *workspace,
                                       const UmiDebugVariableTarget *target, int64_t offset, uint32_t count,
                                       uint32_t timeout_ms, UmiDebugMemoryCapture **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (count == 0U || count > UMI_DEBUG_MEMORY_CAPTURE_CAPACITY || timeout_ms == 0U ||
        offset < -UMI_DEBUG_MEMORY_OFFSET_LIMIT || offset > UMI_DEBUG_MEMORY_OFFSET_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugRuntimeCheckMemory(platform, workspace, target);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    UmiDebugMemoryCapture *capture = calloc(1U, sizeof *capture);
    if (response == NULL || capture == NULL)
    {
        free(response);
        free(capture);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    capture->target = *target;
    capture->offset = offset;
    capture->requested = count;
    uint64_t sequence = 0U;
    status = umi_debug_runtime_request_read_memory(platform->adapter, target->value.memory_reference, offset,
                                                   count, &sequence);
    if (status == UMI_STATUS_OK)
        status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, response);
    if (status == UMI_STATUS_OK && strcmp(response->command, "readMemory") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = UmiDebugMemoryDecode(response->json, count, &capture->result);
    /* Waiting may queue continued/exited/invalidated events. Do not publish
     * bytes against the old stop until the host has processed those events. */
    if (status == UMI_STATUS_OK)
        status = UmiDebugRuntimeCheckMemory(platform, workspace, target);
    if (status == UMI_STATUS_OK)
    {
        *out = capture;
        capture = NULL;
    }
    free(response);
    free(capture);
    return status;
}
