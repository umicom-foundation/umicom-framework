/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/variable_assignment.inc
 * PURPOSE: Keep runtime assignments bound to current evidence and invalidate uncertain captures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/variable_assignment.h"
#include "umicom/language_runtime/json_document.h"
#include "variable_inspection_private.h"

UmiStatus UmiDebugRuntimeCheckVariableAssignment(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target)
{
    if (platform == NULL || workspace == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiDebugWorkspaceVariableService(workspace) != platform->service) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDebugVariableTargetValidate(workspace, target);
    if (status == UMI_STATUS_OK) status = UmiDebugVariableTargetAssignable(target);
    if (status != UMI_STATUS_OK) return status;
    if (!platform->active || !platform->paused || !platform->initialized || platform->adapter == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!platform->capabilities.supports_set_variable) return UMI_STATUS_NOT_IMPLEMENTED;
    UmiDebugRuntimeAdapterSnapshot adapter;
    status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status != UMI_STATUS_OK) return status;
    if (adapter.queued_events != 0U) return UMI_STATUS_BUSY;
    if (adapter.state != UMI_DEBUG_RUNTIME_ADAPTER_PAUSED || platform->active_frame_id > INT32_MAX)
        return UMI_STATUS_INVALID_STATE;
    char frame[32]; (void)snprintf(frame, sizeof frame, "%llu", (unsigned long long)platform->active_frame_id);
    if (strcmp(frame, target->stamp.selectedFrame) != 0 || !target->thread.stopped ||
        target->thread.native_id != platform->active_thread_id ||
        strcmp(target->thread.session_id, platform->active_session_id) != 0) return UMI_STATUS_INVALID_STATE;
    if (platform->revision == UINT64_MAX || umi_debug_variable_registry_revision(
        umi_debug_service_variable(platform->service)) == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}

UmiStatus UmiDebugRuntimeAssignVariable(UmiDebugRuntimePlatform *platform,
    UmiDebugWorkspace *workspace, const UmiDebugVariableTarget *target,
    const char *value, uint32_t timeout_ms, UmiDebugVariableAssignment *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    if (value == NULL || timeout_ms == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < 1024U && value[length] != '\0') ++length;
    if (length == 1024U) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiDebugRuntimeCheckVariableAssignment(platform, workspace, target);
    if (status != UMI_STATUS_OK) return status;
    /* Prepare and validate the complete escaped request before invalidating any
     * displayed evidence. This also rejects malformed UTF-8 in caller input. */
    char arguments[8192]; UmiLanguageRuntimeJsonWriter writer;
    umi_language_runtime_json_writer_init(&writer, arguments, sizeof arguments);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"variablesReference\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, target->containerReference);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"name\":");
    (void)umi_language_runtime_json_writer_string(&writer, target->value.name);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"value\":");
    (void)umi_language_runtime_json_writer_string(&writer, value);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;
    UmiLanguageRuntimeJsonDocument *document = malloc(sizeof *document);
    UmiDebugRuntimeEnvelope *response = malloc(sizeof *response);
    if (document == NULL || response == NULL) { free(document); free(response); return UMI_STATUS_OUT_OF_MEMORY; }
    status = UmiLanguageRuntimeJsonParseComplete(arguments, document);
    free(document);
    if (status != UMI_STATUS_OK) { free(response); return status; }
    /* A failed transport write may already have delivered bytes. Invalidate
     * first, consume this capture once and never retry an uncertain mutation. */
    umi_debug_variable_registry_clear(umi_debug_service_variable(platform->service));
    platform->revision++; out->attempted = 1;
    uint64_t sequence = 0U;
    status = umi_debug_runtime_adapter_send_request(platform->adapter, "setVariable", arguments, target->value.name, &sequence);
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_wait_response(platform->adapter, sequence, timeout_ms, response);
    UmiDebugRuntimeEvaluateResult result;
    if (status == UMI_STATUS_OK && strcmp(response->command, "setVariable") != 0) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiDebugRuntimeDecodeAssignmentValue(response->json, &result);
    free(response);
    UmiDebugRuntimeAdapterSnapshot adapter;
    if (status == UMI_STATUS_OK) status = umi_debug_runtime_adapter_snapshot(platform->adapter, &adapter);
    if (status == UMI_STATUS_OK && adapter.queued_events != 0U) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK) { out->value = result; out->confirmed = 1; }
    return status;
}
