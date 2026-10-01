/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/watch_evaluation.inc
 * PURPOSE: Evaluate explicit retained watch intent only in a current native stopped frame.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/watch_evaluation.h"
#include "../debug/watch_edit_private.h"
#include <stdint.h>
UmiStatus UmiDebugRuntimeEvaluateWatchEdit(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugWatchEdit *edit, uint32_t timeout_ms)
{
    if (platform == NULL || workspace == NULL || edit == NULL || timeout_ms == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiDebugWorkspaceWatchService(workspace) != platform->service) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugWatchEditValidate(workspace, edit);
    if (status != UMI_STATUS_OK) return status;
    if (!platform->active || !platform->paused || !platform->initialized || platform->adapter == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiDebugWatchSnapshot watch;
    status = UmiDebugWatchEditRead(edit, &watch);
    if (status != UMI_STATUS_OK) return status;
    if (!watch.enabled) return UMI_STATUS_INVALID_STATE;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK) return status;
    if (adapter.queued_events != 0U) return UMI_STATUS_BUSY;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED) return UMI_STATUS_INVALID_STATE;
    UmiDebugViewStamp stamp;
    status = UmiDebugWorkspaceViewStamp(workspace, &stamp);
    if (status != UMI_STATUS_OK) return status;
    if (platform->revision == UINT64_MAX || stamp.watches == UINT64_MAX || stamp.selection == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (platform->active_frame_id > INT32_MAX || stamp.selectedFrame[0] == '\0') return UMI_STATUS_INVALID_STATE;
    char frameId[32]; (void)snprintf(frameId, sizeof frameId, "%llu", (unsigned long long)platform->active_frame_id);
    UmiDebugStackFrameSnapshot frame; UmiDebugThreadSnapshot thread;
    status = umi_debug_stack_frame_registry_find(umi_debug_service_stack_frame(platform->service), frameId, &frame);
    if (status == UMI_STATUS_OK) status = umi_debug_thread_registry_find(umi_debug_service_thread(platform->service), frame.thread_id, &thread);
    if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    if (strcmp(stamp.selectedFrame, frame.id) != 0 || strcmp(stamp.selectedThread, thread.id) != 0 ||
        !thread.stopped || thread.native_id != platform->active_thread_id ||
        strcmp(thread.session_id, platform->active_session_id) != 0) return UMI_STATUS_INVALID_STATE;
    char arguments[8192]; UmiLanguageRuntimeJsonWriter writer; uint64_t sequence = 0U;
    umi_language_runtime_json_writer_init(&writer, arguments, sizeof arguments);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"expression\":");
    (void)umi_language_runtime_json_writer_string(&writer, watch.expression);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, platform->active_frame_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"context\":\"watch\"}");
    if (writer.status != UMI_STATUS_OK) return writer.status;
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    if (response == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_debug_runtime_adapter_send_request(platform->adapter, "evaluate", arguments, watch.expression, &sequence);
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, response);
    UmiDebugWatchValue value;
    if (status == UMI_STATUS_OK && strcmp(response->command, "evaluate") != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiDebugRuntimeDecodeWatchValue(response->json, &value);
    free(response);
    if (status != UMI_STATUS_OK) return status;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK) return status;
    /* wait_response buffers events without applying them. Do not publish an
     * apparently current result while a continue/stop/exit may be pending. */
    if (adapter.queued_events != 0U) return UMI_STATUS_BUSY;
    UmiDebugViewStamp now;
    status = UmiDebugWorkspaceViewStamp(workspace, &now);
    if (status != UMI_STATUS_OK) return status;
    if (!UmiDebugViewStampEqual(&stamp, &now)) return UMI_STATUS_BUSY;
    UmiDebugWatchSnapshot replacement = watch;
    memcpy(replacement.value, value.value, sizeof value.value);
    memcpy(replacement.type, value.type, sizeof value.type);
    memcpy(replacement.session_id, platform->active_session_id, sizeof replacement.session_id);
    replacement.valid = 1;
    status = UmiDebugWorkspaceCommitWatch(workspace, &watch, &replacement);
    if (status == UMI_STATUS_OK) platform->revision++;
    return status;
}
