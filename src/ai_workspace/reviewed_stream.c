/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/reviewed_stream.c
 *
 * PURPOSE:
 *   Retain approved local streams through canonical dispatch, native joining and durable workspace settlement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workspace/reviewed_stream.h"
#include "umicom/platform/threading.h"
#include "execution_internal.h"
#include "local_chat_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiAiWorkspaceStream {
    uint64_t owner_thread;
    UmiAiWorkspace *workspace;
    AwExecution *execution;
    UmiAiLocalStream *local;
    AwLocalProviderSettings settings;
    void *provider_instance; /* Compared as identity only; never dereferenced here. */
    bool cancelled;
    bool worker_started;
    UmiAiWorkspaceStreamResult result;
};
static UmiStatus ReviewedOwner(const UmiAiWorkspaceStream *stream)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return stream->owner_thread == umi_thread_current_id()
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
static UmiStatus ReviewedBinding(UmiAiWorkspaceStream *stream)
{
    const UmiAiWorkspaceJob *job = AwExecutionJob(stream->execution);
    UmiStatus status = AwJobBinding(stream->workspace, job, false);
    if (status != UMI_STATUS_OK) return status;
    UmiAiProvider *provider = NULL;
    status = UmiAiRuntimeAdmitProvider(stream->workspace->runtime, job->providerId, 1, &provider);
    if (status != UMI_STATUS_OK) return status;
    AwLocalProviderSettings current;
    status = AwLocalChatProviderSettingsRead(provider, &current);
    if (status != UMI_STATUS_OK) return status;
    if (provider->instance != stream->provider_instance ||
        current.port != stream->settings.port || current.timeout_millis != stream->settings.timeout_millis ||
        current.cancellation != stream->settings.cancellation) return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}
static void ReviewedTransportResult(UmiAiWorkspaceStream *stream)
{
    stream->result.transport_result_available =
        UmiAiLocalStreamReadResult(stream->local, &stream->result.transport) == UMI_STATUS_OK;
    stream->result.worker_started = stream->worker_started;
}
static UmiStatus ReviewedSettle(UmiAiWorkspaceStream *stream, UmiStatus operation)
{
    UmiStatus status = AwExecutionComplete(stream->execution, operation, stream->cancelled);
    stream->result.operation_status = AwExecutionOperationStatus(stream->execution);
    stream->result.publication_status = AwExecutionPublicationStatus(stream->execution);
    return status;
}
UmiStatus UmiAiWorkspaceStreamCreate(UmiAiWorkspace *workspace, const char *job_id,
    UmiAiLocalTransport *transport, const UmiAiWorkspaceStreamOptions *options,
    UmiAiWorkspaceStream **out_stream)
{
    if (workspace == NULL || job_id == NULL || transport == NULL || options == NULL || out_stream == NULL ||
        options->maximum_wire_bytes == 0U ||
        options->maximum_wire_bytes > UMI_AI_LOCAL_STREAM_MAX_WIRE_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!options->allow_network) return UMI_STATUS_PERMISSION_DENIED;
    size_t retained = 0U;
    UmiStatus status = UmiAiLocalTransportRequestCount(transport, &retained);
    if (status != UMI_STATUS_OK) return status;
    status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(job_id, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = AwJobIndex(workspace->state, job_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceJob *job = &workspace->state->jobs[index];
    if (job->state == UMI_AI_WORKSPACE_SUCCEEDED) return UMI_STATUS_ALREADY_EXISTS;
    if (job->state != UMI_AI_WORKSPACE_APPROVED) return UMI_STATUS_INVALID_STATE;
    if (job->kind == UMI_AI_WORKSPACE_TOOL) return UMI_STATUS_NOT_IMPLEMENTED;
    status = AwJobBinding(workspace, job, false);
    if (status != UMI_STATUS_OK) return status;
    UmiAiProvider *provider = NULL;
    status = UmiAiRuntimeAdmitProvider(workspace->runtime, job->providerId, 1, &provider);
    if (status != UMI_STATUS_OK) return status;
    AwLocalProviderSettings settings;
    status = AwLocalChatProviderSettingsRead(provider, &settings);
    if (status != UMI_STATUS_OK) return status;
    if (UmiAiWorkspaceCancellationIsRequested(settings.cancellation)) return UMI_STATUS_CANCELLED;
    if (workspace->state->revision == 0U) return UMI_STATUS_INVALID_STATE;
    UmiAiWorkspaceStream *stream = calloc(1U, sizeof(*stream));
    if (stream == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    stream->owner_thread = umi_thread_current_id();
    stream->workspace = workspace;
    stream->settings = settings;
    stream->provider_instance = provider->instance;
    stream->result.operation_status = UMI_STATUS_BUSY;
    stream->result.publication_status = UMI_STATUS_BUSY;
    status = AwExecutionPrepare(workspace, job_id, NULL, &stream->execution);
    if (status != UMI_STATUS_OK) { free(stream); return status; }
    UmiAiLocalStreamOptions local = {
        .port = settings.port,
        .timeout_millis = settings.timeout_millis,
        .maximum_text_bytes = sizeof(AwExecutionResponse(stream->execution)->text) - 1U,
        .maximum_wire_bytes = options->maximum_wire_bytes,
        /* These private tokens identify the frozen workspace admission. No
         * provider/model authentication or global cross-workspace ID is implied.
         * The raw owner accepts events only from its own retained producer. */
        .identity = {workspace->state->revision, workspace->state->revision},
        .allow_network = true
    };
    status = UmiAiLocalStreamCreate(transport, AwExecutionRequest(stream->execution), &local, &stream->local);
    if (status != UMI_STATUS_OK) {
        /* Prepare owns this still-unstarted execution exclusively. No external
         * callback can alter its lease, so prepared release must succeed. */
        (void)AwExecutionRelease(&stream->execution);
        free(stream);
        return status;
    }
    *out_stream = stream;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceStreamCancel(UmiAiWorkspaceStream *stream)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    if (AwExecutionGetPhase(stream->execution) == AW_EXECUTION_SETTLED) return UMI_STATUS_OK;
    stream->cancelled = true;
    return UmiAiLocalStreamCancel(stream->local);
}
static UmiStatus ReviewedObserveCancellation(UmiAiWorkspaceStream *stream)
{
    if (UmiAiWorkspaceCancellationIsRequested(stream->settings.cancellation))
        return UmiAiWorkspaceStreamCancel(stream);
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceStreamStart(UmiAiWorkspaceStream *stream)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    if (AwExecutionGetPhase(stream->execution) != AW_EXECUTION_PREPARED) return UMI_STATUS_INVALID_STATE;
    status = ReviewedObserveCancellation(stream);
    if (status != UMI_STATUS_OK) return status;
    if (stream->cancelled) return UMI_STATUS_CANCELLED;
    status = ReviewedBinding(stream);
    if (status != UMI_STATUS_OK) return status;
    status = AwExecutionStart(stream->execution);
    if (status != UMI_STATUS_OK) return status;
    /* Recheck immediately after durable publication, matching synchronous
     * Runtime dispatch. No network request precedes this committed state. */
    status = ReviewedBinding(stream);
    if (status == UMI_STATUS_OK) status = ReviewedObserveCancellation(stream);
    if (status == UMI_STATUS_OK && stream->cancelled) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) status = UmiAiLocalStreamStart(stream->local);
    if (status == UMI_STATUS_OK) {
        stream->worker_started = true;
        return UMI_STATUS_OK;
    }
    /* The caller already owns the controller. A start failure can therefore
     * retain both publication evidence and concrete cleanup responsibility. */
    (void)UmiAiLocalStreamCancel(stream->local);
    ReviewedTransportResult(stream);
    return ReviewedSettle(stream, status);
}
UmiStatus UmiAiWorkspaceStreamPoll(UmiAiWorkspaceStream *stream,
    UmiAiWorkspaceStreamSnapshot *out_snapshot)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    if (out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ReviewedObserveCancellation(stream);
    if (status != UMI_STATUS_OK) return status;
    UmiAiWorkspaceStreamSnapshot snapshot = {0};
    status = UmiAiLocalStreamPoll(stream->local, &snapshot.transport);
    if (status != UMI_STATUS_OK) return status;
    switch (AwExecutionGetPhase(stream->execution)) {
        case AW_EXECUTION_PREPARED: snapshot.phase = UMI_AI_WORKSPACE_STREAM_PREPARED; break;
        case AW_EXECUTION_RUNNING: snapshot.phase = UMI_AI_WORKSPACE_STREAM_RUNNING; break;
        case AW_EXECUTION_SETTLED: snapshot.phase = UMI_AI_WORKSPACE_STREAM_SETTLED; break;
    }
    snapshot.cancellation_requested = stream->cancelled;
    snapshot.worker_started = stream->worker_started;
    *out_snapshot = snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceStreamCopyText(UmiAiWorkspaceStream *stream, char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiLocalStreamCopyText(stream->local, out_text, out_snapshot);
}
static UmiStatus ReviewedFinish(UmiAiWorkspaceStream *stream, bool wait)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    AwExecutionPhase phase = AwExecutionGetPhase(stream->execution);
    if (phase == AW_EXECUTION_SETTLED) return UMI_STATUS_OK;
    if (phase != AW_EXECUTION_RUNNING) return UMI_STATUS_INVALID_STATE;
    status = ReviewedObserveCancellation(stream);
    if (status != UMI_STATUS_OK) return status;
    status = wait ? UmiAiLocalStreamJoin(stream->local) : UmiAiLocalStreamTryJoin(stream->local);
    if (status != UMI_STATUS_OK) return status;
    ReviewedTransportResult(stream);
    UmiStatus operation = stream->result.transport_result_available
        ? stream->result.transport.operation_status : UMI_STATUS_INTERNAL_ERROR;
    if (operation == UMI_STATUS_OK) operation = stream->result.transport.observation_status;
    if (operation == UMI_STATUS_OK) operation = stream->result.transport.terminal_publication_status;
    if (operation == UMI_STATUS_OK) {
        char *text = NULL;
        UmiAiResponseStreamSnapshot snapshot;
        operation = UmiAiLocalStreamCopyText(stream->local, &text, &snapshot);
        UmiAiResponse *response = AwExecutionResponse(stream->execution);
        const UmiAiWorkspaceJob *job = AwExecutionJob(stream->execution);
        if (operation == UMI_STATUS_OK &&
            (snapshot.state != UMI_AI_RESPONSE_STREAM_COMPLETE || snapshot.pending_bytes != 0U ||
             snapshot.text_bytes >= sizeof(response->text) || strlen(text) != snapshot.text_bytes))
            operation = UMI_STATUS_PARSE_ERROR;
        if (operation == UMI_STATUS_OK)
            operation = AwTextCopy(response->request_id, sizeof(response->request_id), job->id, false);
        if (operation == UMI_STATUS_OK)
            operation = AwTextCopy(response->provider_id, sizeof(response->provider_id), job->providerId, false);
        if (operation == UMI_STATUS_OK)
            operation = AwTextCopy(response->model_id, sizeof(response->model_id), job->modelId, false);
        if (operation == UMI_STATUS_OK) {
            memcpy(response->text, text, snapshot.text_bytes + 1U);
            response->finish_reason = stream->result.transport.finish_reason;
            response->usage = stream->result.transport.usage;
        }
        UmiAiResponseStreamTextFree(text);
    }
    /* Cancellation can arrive while the owner joins or copies final text. Its
     * decision is observed again before durable publication of the outcome. */
    if (UmiAiWorkspaceCancellationIsRequested(stream->settings.cancellation))
        stream->cancelled = true;
    (void)ReviewedSettle(stream, operation);
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceStreamTryFinish(UmiAiWorkspaceStream *stream)
{ return ReviewedFinish(stream, false); }
UmiStatus UmiAiWorkspaceStreamFinish(UmiAiWorkspaceStream *stream)
{ return ReviewedFinish(stream, true); }
UmiStatus UmiAiWorkspaceStreamReadResult(UmiAiWorkspaceStream *stream,
    UmiAiWorkspaceStreamResult *out_result)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (AwExecutionGetPhase(stream->execution) != AW_EXECUTION_SETTLED) return UMI_STATUS_BUSY;
    *out_result = stream->result;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceStreamDestroy(UmiAiWorkspaceStream **in_out_stream)
{
    if (in_out_stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiWorkspaceStream *stream = *in_out_stream;
    if (stream == NULL) return UMI_STATUS_OK;
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    if (AwExecutionGetPhase(stream->execution) == AW_EXECUTION_RUNNING) return UMI_STATUS_BUSY;
    status = UmiAiLocalStreamDestroy(&stream->local);
    if (status != UMI_STATUS_OK) return status;
    status = AwExecutionRelease(&stream->execution);
    if (status != UMI_STATUS_OK) return status;
    free(stream);
    *in_out_stream = NULL;
    return UMI_STATUS_OK;
}

/* Preserve the controller ownership check and delegate byte boundaries to
 * the shared response-stream owner; do not read producer protocol metadata. */
UmiStatus UmiAiWorkspaceStreamCopyTextRange(UmiAiWorkspaceStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    UmiStatus status = ReviewedOwner(stream);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiLocalStreamCopyTextRange(stream->local, offset, maximum_bytes, out_text, out_copied, out_snapshot);
}
