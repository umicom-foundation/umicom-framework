/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_workspace_rename.inc
 * PURPOSE: Connect optional multi-draft rename to copied worker inputs and complete document review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after the form's state and owner helpers. Only this GTK-thread
 * capture follows document owners; the worker sees copied source and metadata. */
static UmiStatus CompletionWorkspaceCapture(UmiGtk4Adapter *owner, CompletionReview *review,
                                            CompletionReviewWork *work)
{
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM];
    size_t count = 1U;
    ids[0] = work->source.document_id;
    size_t open_count = umi_document_coordinator_count(owner->edit_coordinator);
    for (size_t i = 0U; i < open_count; ++i)
    {
        UmiDocumentWorkingCopySnapshot snapshot;
        UmiStatus status = umi_document_coordinator_at(owner->edit_coordinator, i, &snapshot);
        if (status != UMI_STATUS_OK)
            return status;
        if (snapshot.document_id == ids[0] || strcmp(snapshot.language_id, work->source.language_id) != 0)
            continue;
        if (count == UMI_DOCUMENT_SOURCE_WORKSPACE_MAXIMUM)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ids[count++] = snapshot.document_id;
    }
    UmiStatus status =
        UmiDocumentSourceWorkspaceCreate(owner->edit_coordinator, ids, count, &review->workspace_sources);
    if (status != UMI_STATUS_OK)
        return status;
    if (count > 1U)
    {
        work->additional_sources = g_try_new0(UmiLanguageQuerySource, count - 1U);
        work->additional_documents = g_try_new0(UmiDocumentSourceRequestSummary, count - 1U);
        if (work->additional_sources == NULL || work->additional_documents == NULL)
            return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t i = 1U; status == UMI_STATUS_OK && i < count; ++i)
    {
        UmiDocumentSourceRequestSummary *document = &work->additional_documents[i - 1U];
        const char *source = NULL;
        size_t bytes = 0U;
        status = UmiDocumentSourceWorkspaceAt(review->workspace_sources, i, document);
        if (status == UMI_STATUS_OK && strcmp(document->language_id, work->source.language_id) != 0)
            status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceWorkspaceRead(review->workspace_sources, i, &source, &bytes);
        if (status == UMI_STATUS_OK && bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        char *copy = status == UMI_STATUS_OK ? g_try_malloc(bytes + 1U) : NULL;
        if (status == UMI_STATUS_OK && copy == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK)
        {
            memcpy(copy, source, bytes);
            copy[bytes] = '\0';
            work->additional_sources[i - 1U] =
                (UmiLanguageQuerySource){document->uri, document->language_id, copy, bytes};
            ++work->additional_count;
        }
    }
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceWorkspaceCheck(owner->edit_coordinator, review->workspace_sources);
    return status;
}
/* Native cleanup has finished before this callback. Recheck the original
 * dependency capture, then transfer a fully staged model to a separate review.
 * Opening the comparison is not approval and changes no document text. */
static void CompletionWorkspaceComplete(GtkWidget *window, CompletionReview *review,
                                        CompletionReviewWork *work)
{
    if (review == NULL || review->pending != work)
        return;
    review->pending = NULL;
    g_object_set_data(G_OBJECT(window), "umicom-completion-pending", NULL);
    review->rendering = 1;
    UmiGtk4Adapter *owner = CompletionReviewOwner(window, review);
    UmiStatus status = owner == NULL || work->stale || umi_cancellation_token_is_requested(work->cancel)
                           ? UMI_STATUS_CANCELLED
                           : work->status;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestCheck(owner->edit_coordinator, review->source);
    UmiSourceWorkspaceEditReview *model = NULL;
    const int32_t source_version = 1;
    if (status == UMI_STATUS_OK)
        status =
            UmiSourceWorkspaceEditReviewCreate(owner->edit_coordinator, &review->workspace_sources,
                                               &work->workspace_edits, &source_version, work->cancel, &model);
    if (status == UMI_STATUS_OK)
        status = UmiGtk4AdapterReviewWorkspaceEdits(owner, &model);
    UmiSourceWorkspaceEditReviewDestroy(model);
    if (CompletionReviewOwner(window, review) != NULL)
    {
        char *message =
            status == UMI_STATUS_OK
                ? g_strdup(
                      "The complete workspace review is open. Review each changed draft and any required "
                      "explanations, then explicitly approve the complete edit. No files are saved.")
                : g_strdup_printf(
                      "Workspace rename: %s. Close: %s. Shutdown: %s. No partial edit was applied. All "
                      "returned targets must be captured open drafts; capture again if any source changed.",
                      umi_status_text(status), umi_status_text(work->report.close_status),
                      umi_status_text(work->report.shutdown_status));
        gtk_label_set_text(GTK_LABEL(review->message), message);
        g_free(message);
        gtk_widget_set_sensitive(review->preview, FALSE);
        gtk_widget_set_sensitive(review->apply, FALSE);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
    }
    review->rendering = 0;
    if (CompletionReviewOwner(window, review) != NULL)
        gtk_widget_set_sensitive(review->request, TRUE);
}

/* Keep the action catalogue owned by the request form. Only the explicitly
 * selected, fully decoded edit transfers into the complete review model. */
static void CompletionWorkspaceActionPreview(GtkWidget *window, CompletionReview *review,
                                             UmiGtk4Adapter *owner)
{
    review->rendering = 1;
    UmiStatus status = UmiGtk4AdapterReplacementReviewBusy(owner)
                           ? UMI_STATUS_BUSY
                           : UmiDocumentSourceRequestCheck(owner->edit_coordinator, review->source);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceWorkspaceCheck(owner->edit_coordinator, review->workspace_sources);
    UmiLanguageWorkspaceEditCatalogue *changes = NULL;
    UmiSourceWorkspaceEditReview *model = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCodeActionCatalogueReadEdits(
            review->actions, gtk_drop_down_get_selected(GTK_DROP_DOWN(review->choice)), NULL, &changes);
    const int32_t source_version = 1;
    if (status == UMI_STATUS_OK)
        status = UmiSourceWorkspaceEditReviewCreate(owner->edit_coordinator, &review->workspace_sources,
                                                    &changes, &source_version, NULL, &model);
    if (status == UMI_STATUS_OK)
        status = UmiGtk4AdapterReviewWorkspaceEdits(owner, &model);
    UmiSourceWorkspaceEditReviewDestroy(model);
    UmiLanguageWorkspaceEditCatalogueDestroy(changes);
    if (CompletionReviewOwner(window, review) != NULL)
    {
        char *message =
            status == UMI_STATUS_OK
                ? g_strdup(
                      "The selected action is open in complete workspace review. Inspect every changed draft "
                      "and required explanation, then approve the complete edit. Save remains separate.")
                : g_strdup_printf("Cannot review the complete action: %s. No edit subset was applied. "
                                  "Targets must be captured writable drafts; commands, deferred actions and "
                                  "resource operations remain unavailable.",
                                  umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(review->message), message);
        g_free(message);
        gtk_widget_set_sensitive(review->apply, FALSE);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
        if (review->workspace_sources == NULL)
            gtk_widget_set_sensitive(review->preview, FALSE);
    }
    review->rendering = 0;
}
