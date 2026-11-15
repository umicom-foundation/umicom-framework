/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ai_workspace_reviewed_stream.inc
 *
 * PURPOSE:
 *   Present reviewed local streams in existing GTK controls while retaining native and workspace ownership through settlement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* This fragment is part of the panel owner, not another provider or transport.
 * All widget updates happen on GTK's owner thread; producer text is copied. */
static void AiStreamControls(UmiAiWorkspaceGtkPanel *panel, bool running)
{
    gtk_widget_set_sensitive(panel->forms, !running);
    gtk_widget_set_sensitive(GTK_WIDGET(panel->cancel), TRUE);
    gtk_button_set_label(panel->cancel, running ? "Stop current request" : "Cancel selected job");
}
static void AiStreamDraft(UmiAiWorkspaceGtkPanel *panel, const char *text, bool rejected)
{
    char *display = g_strconcat(rejected
        ? "Temporary draft — generation or saving did not succeed. This is not a saved answer.\n\n"
        : "Streaming draft — incomplete and not yet validated or saved.\n\n", text, NULL);
    gtk_text_buffer_set_text(panel->review, display, -1);
    g_free(display);
}
static UmiStatus AiStreamStopAndRelease(UmiAiWorkspaceGtkPanel *panel)
{
    if (panel->stream == NULL) return UMI_STATUS_OK;
    UmiStatus status = UmiAiWorkspaceStreamCancel(panel->stream);
    if (status != UMI_STATUS_OK) return status;
    UmiAiWorkspaceStreamSnapshot snapshot;
    status = UmiAiWorkspaceStreamPoll(panel->stream, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_RUNNING) {
        status = UmiAiWorkspaceStreamFinish(panel->stream);
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiAiWorkspaceStreamDestroy(&panel->stream);
}
/* Keep a callback lease while copied text and status notifications can reenter checked panel retirement. The former implementation is retained for owner review. */
#if 0
static gboolean AiPollStream(gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data;
    if (panel->closed || panel->stream == NULL) {
        panel->pollSource = 0U;
        return G_SOURCE_REMOVE;
    }
    UmiAiWorkspaceStreamSnapshot snapshot = {0};
    UmiStatus status = UmiAiWorkspaceStreamPoll(panel->stream, &snapshot);
    if (status != UMI_STATUS_OK) {
        (void)UmiAiWorkspaceStreamCancel(panel->stream);
        if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
        gtk_label_set_text(panel->message, "Stream observation failed. Ownership is retained while cancellation and cleanup are retried.");
        return G_SOURCE_CONTINUE;
    }
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_RUNNING) {
        char *text = NULL;
        UmiAiResponseStreamSnapshot text_snapshot;
        status = UmiAiWorkspaceStreamCopyText(panel->stream, &text, &text_snapshot);
        if (status == UMI_STATUS_OK) AiStreamDraft(panel, text, false);
        else {
            if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
        }
        UmiAiResponseStreamTextFree(text);
        status = UmiAiWorkspaceStreamTryFinish(panel->stream);
        if (status == UMI_STATUS_NOT_IMPLEMENTED &&
            snapshot.transport.state == UMI_AI_LOCAL_STREAM_FINISHED) {
            /* A platform without try-join can join only after the producer has
             * published completion. Native thread teardown may still block.
             * No join is attempted while its HTTP exchange is still active. */
            status = UmiAiWorkspaceStreamFinish(panel->stream);
        }
        if (status == UMI_STATUS_BUSY || status == UMI_STATUS_NOT_IMPLEMENTED)
            return G_SOURCE_CONTINUE;
        if (status != UMI_STATUS_OK) {
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
            gtk_label_set_text(panel->message, "The native request has not released safely. Controls and services remain retained; cleanup will be retried.");
            return G_SOURCE_CONTINUE;
        }
        snapshot.phase = UMI_AI_WORKSPACE_STREAM_SETTLED;
    }
    UmiStatus operation = panel->streamObservationStatus;
    UmiStatus publication = UMI_STATUS_OK;
    char *partial = NULL;
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_SETTLED) {
        UmiAiWorkspaceStreamResult result;
        status = UmiAiWorkspaceStreamReadResult(panel->stream, &result);
        if (status != UMI_STATUS_OK) { Message(panel, status); return G_SOURCE_CONTINUE; }
        if (operation == UMI_STATUS_OK) operation = result.operation_status;
        publication = result.publication_status;
        if (operation != UMI_STATUS_OK || publication != UMI_STATUS_OK) {
            UmiAiResponseStreamSnapshot text_snapshot;
            (void)UmiAiWorkspaceStreamCopyText(panel->stream, &partial, &text_snapshot);
        }
    }
    status = UmiAiWorkspaceStreamDestroy(&panel->stream);
    if (status != UMI_STATUS_OK) {
        UmiAiResponseStreamTextFree(partial);
        gtk_label_set_text(panel->message, "Result settled, but native release failed. The request and its services remain retained for cleanup.");
        return G_SOURCE_CONTINUE;
    }
    panel->pollSource = 0U;
    AiStreamControls(panel, false);
    Refresh(panel);
    if (partial != NULL && partial[0] != '\0') AiStreamDraft(panel, partial, true);
    UmiAiResponseStreamTextFree(partial);
    char *message = g_strdup_printf("Generation: %s. Save: %s.%s",
        umi_status_text(operation), umi_status_text(publication),
        publication == UMI_STATUS_OK ? " Review the recorded job before using its draft." :
        " The saved outcome is uncertain. Reload and inspect it; generation was not repeated.");
    gtk_label_set_text(panel->message, message);
    g_free(message);
    return G_SOURCE_REMOVE;
}

#endif

/* One bounded range per owner-thread tick avoids replacing an unchanged draft.
 * GTK may emit nested signals during insertion; the surrounding poll callback
 * retains its existing retirement guard until this helper has returned. */
static UmiStatus AiStreamAppend(UmiAiWorkspaceGtkPanel *panel)
{
    char *text = NULL;
    size_t copied = 0U;
    UmiAiResponseStreamSnapshot snapshot;
    UmiStatus status = UmiAiWorkspaceStreamCopyTextRange(panel->stream,
        panel->streamDisplayedBytes, 4096U, &text, &copied, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (copied != 0U) {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(panel->review, &end);
        gtk_text_buffer_insert(panel->review, &end, text, (int)copied);
        panel->streamDisplayedBytes += copied;
    }
    UmiAiResponseStreamTextFree(text);
    return UMI_STATUS_OK;
}

/* Append bounded complete UTF-8 ranges in the current draft instead of copying
 * its full prefix on every tick. Keep the prior presentation for owner review. */
#if 0
static gboolean AiPollStreamTick(gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data;
    if (panel->closed || panel->stream == NULL) {
        panel->pollSource = 0U;
        return G_SOURCE_REMOVE;
    }
    UmiAiWorkspaceStreamSnapshot snapshot = {0};
    UmiStatus status = UmiAiWorkspaceStreamPoll(panel->stream, &snapshot);
    if (status != UMI_STATUS_OK) {
        (void)UmiAiWorkspaceStreamCancel(panel->stream);
        if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
        gtk_label_set_text(panel->message, "Stream observation failed. Ownership is retained while cancellation and cleanup are retried.");
        return G_SOURCE_CONTINUE;
    }
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_RUNNING) {
        char *text = NULL;
        UmiAiResponseStreamSnapshot text_snapshot;
        status = UmiAiWorkspaceStreamCopyText(panel->stream, &text, &text_snapshot);
        if (status == UMI_STATUS_OK) AiStreamDraft(panel, text, false);
        else {
            if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
        }
        UmiAiResponseStreamTextFree(text);
        status = UmiAiWorkspaceStreamTryFinish(panel->stream);
        if (status == UMI_STATUS_NOT_IMPLEMENTED &&
            snapshot.transport.state == UMI_AI_LOCAL_STREAM_FINISHED) {
            /* A platform without try-join can join only after the producer has
             * published completion. Native thread teardown may still block.
             * No join is attempted while its HTTP exchange is still active. */
            status = UmiAiWorkspaceStreamFinish(panel->stream);
        }
        if (status == UMI_STATUS_BUSY || status == UMI_STATUS_NOT_IMPLEMENTED)
            return G_SOURCE_CONTINUE;
        if (status != UMI_STATUS_OK) {
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
            gtk_label_set_text(panel->message, "The native request has not released safely. Controls and services remain retained; cleanup will be retried.");
            return G_SOURCE_CONTINUE;
        }
        snapshot.phase = UMI_AI_WORKSPACE_STREAM_SETTLED;
    }
    UmiStatus operation = panel->streamObservationStatus;
    UmiStatus publication = UMI_STATUS_OK;
    char *partial = NULL;
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_SETTLED) {
        UmiAiWorkspaceStreamResult result;
        status = UmiAiWorkspaceStreamReadResult(panel->stream, &result);
        if (status != UMI_STATUS_OK) { Message(panel, status); return G_SOURCE_CONTINUE; }
        if (operation == UMI_STATUS_OK) operation = result.operation_status;
        publication = result.publication_status;
        if (operation != UMI_STATUS_OK || publication != UMI_STATUS_OK) {
            UmiAiResponseStreamSnapshot text_snapshot;
            (void)UmiAiWorkspaceStreamCopyText(panel->stream, &partial, &text_snapshot);
        }
    }
    status = UmiAiWorkspaceStreamDestroy(&panel->stream);
    if (status != UMI_STATUS_OK) {
        UmiAiResponseStreamTextFree(partial);
        gtk_label_set_text(panel->message, "Result settled, but native release failed. The request and its services remain retained for cleanup.");
        return G_SOURCE_CONTINUE;
    }
    panel->pollSource = 0U;
    AiStreamControls(panel, false);
    Refresh(panel);
    if (partial != NULL && partial[0] != '\0') AiStreamDraft(panel, partial, true);
    UmiAiResponseStreamTextFree(partial);
/* An abandoned prepared owner performed no final publication; report that state without displaying an invented successful save. The former implementation is retained for owner review. */
#if 0
    char *message = g_strdup_printf("Generation: %s. Save: %s.%s",
        umi_status_text(operation), umi_status_text(publication),
        publication == UMI_STATUS_OK ? " Review the recorded job before using its draft." :
        " The saved outcome is uncertain. Reload and inspect it; generation was not repeated.");
#endif
    char *message = snapshot.phase == UMI_AI_WORKSPACE_STREAM_PREPARED
        ? g_strdup_printf("Request did not start: %s. Approval is preserved; no generated result was saved.",
            umi_status_text(operation))
        : g_strdup_printf("Generation: %s. Save: %s.%s",
            umi_status_text(operation), umi_status_text(publication),
            publication == UMI_STATUS_OK ? " Review the recorded job before using its draft." :
            " The saved outcome is uncertain. Reload and inspect it; generation was not repeated.");
    gtk_label_set_text(panel->message, message);
    g_free(message);
    return G_SOURCE_REMOVE;
}

#endif
static gboolean AiPollStreamTick(gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data;
    if (panel->closed || panel->stream == NULL) {
        panel->pollSource = 0U;
        return G_SOURCE_REMOVE;
    }
    UmiAiWorkspaceStreamSnapshot snapshot = {0};
    UmiStatus status = UmiAiWorkspaceStreamPoll(panel->stream, &snapshot);
    if (status != UMI_STATUS_OK) {
        (void)UmiAiWorkspaceStreamCancel(panel->stream);
        if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
        gtk_label_set_text(panel->message, "Stream observation failed. Ownership is retained while cancellation and cleanup are retried.");
        return G_SOURCE_CONTINUE;
    }
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_RUNNING) {
        status = AiStreamAppend(panel);
        if (status != UMI_STATUS_OK) {
            if (panel->streamObservationStatus == UMI_STATUS_OK) panel->streamObservationStatus = status;
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
        }
        status = UmiAiWorkspaceStreamTryFinish(panel->stream);
        if (status == UMI_STATUS_NOT_IMPLEMENTED &&
            snapshot.transport.state == UMI_AI_LOCAL_STREAM_FINISHED) {
            /* A platform without try-join can join only after the producer has
             * published completion. Native thread teardown may still block.
             * No join is attempted while its HTTP exchange is still active. */
            status = UmiAiWorkspaceStreamFinish(panel->stream);
        }
        if (status == UMI_STATUS_BUSY || status == UMI_STATUS_NOT_IMPLEMENTED)
            return G_SOURCE_CONTINUE;
        if (status != UMI_STATUS_OK) {
            (void)UmiAiWorkspaceStreamCancel(panel->stream);
            gtk_label_set_text(panel->message, "The native request has not released safely. Controls and services remain retained; cleanup will be retried.");
            return G_SOURCE_CONTINUE;
        }
        snapshot.phase = UMI_AI_WORKSPACE_STREAM_SETTLED;
    }
    UmiStatus operation = panel->streamObservationStatus;
    UmiStatus publication = UMI_STATUS_OK;
    char *partial = NULL;
    if (snapshot.phase == UMI_AI_WORKSPACE_STREAM_SETTLED) {
        UmiAiWorkspaceStreamResult result;
        status = UmiAiWorkspaceStreamReadResult(panel->stream, &result);
        if (status != UMI_STATUS_OK) { Message(panel, status); return G_SOURCE_CONTINUE; }
        if (operation == UMI_STATUS_OK) operation = result.operation_status;
        publication = result.publication_status;
        if (operation != UMI_STATUS_OK || publication != UMI_STATUS_OK) {
            UmiAiResponseStreamSnapshot text_snapshot;
            (void)UmiAiWorkspaceStreamCopyText(panel->stream, &partial, &text_snapshot);
        }
    }
    status = UmiAiWorkspaceStreamDestroy(&panel->stream);
    if (status != UMI_STATUS_OK) {
        UmiAiResponseStreamTextFree(partial);
        gtk_label_set_text(panel->message, "Result settled, but native release failed. The request and its services remain retained for cleanup.");
        return G_SOURCE_CONTINUE;
    }
    panel->pollSource = 0U;
    AiStreamControls(panel, false);
    Refresh(panel);
    if (partial != NULL && partial[0] != '\0') AiStreamDraft(panel, partial, true);
    UmiAiResponseStreamTextFree(partial);
/* An abandoned prepared owner performed no final publication; report that state without displaying an invented successful save. The former implementation is retained for owner review. */
#if 0
    char *message = g_strdup_printf("Generation: %s. Save: %s.%s",
        umi_status_text(operation), umi_status_text(publication),
        publication == UMI_STATUS_OK ? " Review the recorded job before using its draft." :
        " The saved outcome is uncertain. Reload and inspect it; generation was not repeated.");
#endif
    char *message = snapshot.phase == UMI_AI_WORKSPACE_STREAM_PREPARED
        ? g_strdup_printf("Request did not start: %s. Approval is preserved; no generated result was saved.",
            umi_status_text(operation))
        : g_strdup_printf("Generation: %s. Save: %s.%s",
            umi_status_text(operation), umi_status_text(publication),
            publication == UMI_STATUS_OK ? " Review the recorded job before using its draft." :
            " The saved outcome is uncertain. Reload and inspect it; generation was not repeated.");
    gtk_label_set_text(panel->message, message);
    g_free(message);
    return G_SOURCE_REMOVE;
}
static gboolean AiPollStream(gpointer data)
{
    UmiAiWorkspaceGtkPanel *panel = data;
    panel->streamCallbackActive = true;
    gboolean result = AiPollStreamTick(data);
    panel->streamCallbackActive = false;
    return result;
}
/* true means this request was handled or refused by reviewed streaming.
 * Only NOT_IMPLEMENTED permits the established complete-response route. */
static bool AiTryStream(UmiAiWorkspaceGtkPanel *panel)
{
    UmiAiWorkspaceCancellationReset(panel->cancellation);
    UmiAiWorkspaceStreamOptions options = {
        .maximum_wire_bytes = 16U * 1024U * 1024U, .allow_network = true
    };
    UmiStatus status = UmiAiWorkspaceStreamCreate(panel->workspace, panel->selectedId,
        panel->transport, &options, &panel->stream);
    if (status == UMI_STATUS_NOT_IMPLEMENTED) return false;
    if (status != UMI_STATUS_OK) { Message(panel, status); return true; }
    panel->streamObservationStatus = UMI_STATUS_OK;
    panel->streamDisplayedBytes = 0U;
    AiStreamControls(panel, true);
    AiStreamDraft(panel, "", false);
    status = UmiAiWorkspaceStreamStart(panel->stream);
    if (status != UMI_STATUS_OK) panel->streamObservationStatus = status;
    panel->pollSource = g_timeout_add(100U, AiPollStream, panel);
    if (panel->pollSource == 0U) {
        /* Scheduling failure must not strand an active producer. This error
         * path explicitly cancels and blocks; borrowed storage survives refusal. */
        UmiStatus release = AiStreamStopAndRelease(panel);
        if (release == UMI_STATUS_OK) {
            AiStreamControls(panel, false); Refresh(panel); Message(panel, UMI_STATUS_UNAVAILABLE);
        } else {
            gtk_label_set_text(panel->message, "Completion monitoring could not start. The request was cancelled; services remain retained until safe shutdown.");
        }
        return true;
    }
    gtk_label_set_text(panel->message, status == UMI_STATUS_OK
        ? "Streaming the approved request. Text remains a draft until validation and saving finish."
        : "The request could not start. Retaining its outcome while cleanup completes.");
    return true;
}
UmiStatus UmiAiWorkspaceGtkPanelEnableLocalStreaming(UmiAiWorkspaceGtkPanel *panel,
    UmiAiLocalTransport *transport)
{
    if (panel == NULL || transport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (panel->ownerThread != umi_thread_current_id() || panel->closed) return UMI_STATUS_INVALID_STATE;
    if (!Available(panel)) return UMI_STATUS_BUSY;
    size_t requests = 0U;
    UmiStatus status = UmiAiLocalTransportRequestCount(transport, &requests);
    if (status != UMI_STATUS_OK) return status;
    panel->transport = transport;
    return UMI_STATUS_OK;
}
