/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_proposal_review.inc
 * PURPOSE: Present selected-code proposals with explicit preview, approval and the existing document binding lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/proposal.h"
#include "umicom/ui/gtk4/interaction_recording.h"

typedef struct DocumentProposalReview
{
    GObject *lifetime;
    UmiDocumentProposal *proposal;
    GtkWidget *source, *input, *message, *comparison, *approve, *preview, *apply, *cancel;
    uint64_t generation, presented_revision;
    int closed, rendering;
} DocumentProposalReview;
static UmiGtk4Adapter *DocumentProposalOwner(GtkWidget *window, DocumentProposalReview *review)
{
    if (review == NULL || review->closed || g_object_get_data(review->lifetime, "proposal-dialog") != window)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(review->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void DocumentProposalReviewFree(gpointer data)
{
    DocumentProposalReview *review = data;
    UmiDocumentProposalDestroy(review->proposal);
    g_clear_object(&review->lifetime);
    g_clear_object(&review->source);
    g_clear_object(&review->input);
    g_clear_object(&review->message);
    g_clear_object(&review->comparison);
    g_clear_object(&review->approve);
    g_clear_object(&review->preview);
    g_clear_object(&review->apply);
    g_clear_object(&review->cancel);
    g_free(review);
}
static void DocumentProposalRemoveComparison(DocumentProposalReview *review)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(review->comparison)) != NULL)
        gtk_box_remove(GTK_BOX(review->comparison), child);
}
/* The window reference and owned widget references survive native notifications.
 * Invalidate the token first so retained controls cannot use a retired host. */
static void DocumentProposalClose(GtkWidget *window, int notify, UmiStatus status)
{
    g_object_ref(window);
    DocumentProposalReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-proposal");
    if (review != NULL && !review->closed)
    {
        GObject *lifetime = g_object_ref(review->lifetime);
        UmiGtk4Adapter *owner = DocumentProposalOwner(window, review);
        review->closed = 1;
        review->presented_revision = 0U;
        if (g_object_get_data(lifetime, "proposal-dialog") == window)
            g_object_set_data(lifetime, "proposal-dialog", NULL);
        UmiDocumentProposalDestroy(review->proposal);
        review->proposal = NULL;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->source)), "", 0);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->input)), "", 0);
        DocumentProposalRemoveComparison(review);
        gtk_widget_set_sensitive(review->apply, FALSE);
        gtk_window_destroy(GTK_WINDOW(window));
        /* Completion is last because refreshing documents may detach or destroy
         * Studio. No later statement dereferences that application owner. */
        if (notify && owner != NULL && g_object_get_data(lifetime, "adapter") == owner &&
            owner->window != NULL && UmiGtk4WindowIsOpen(owner->window))
            (void)EditFinished(owner, status);
        g_object_unref(lifetime);
    }
    else
        gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
}
static void DocumentProposalRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    DocumentProposalReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-proposal");
    if (review != NULL && !review->closed)
        DocumentProposalClose(window, 1, UMI_STATUS_CANCELLED);
}
static void DocumentProposalParentRemoved(GtkWidget *parent, gpointer data)
{
    (void)parent;
    DocumentProposalClose(GTK_WIDGET(data), 0, UMI_STATUS_CANCELLED);
}
static void DocumentProposalDetach(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "proposal-dialog");
    if (window != NULL)
        DocumentProposalClose(window, 0, UMI_STATUS_CANCELLED);
}
static gboolean DocumentProposalClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    DocumentProposalClose(GTK_WIDGET(window), 1, UMI_STATUS_CANCELLED);
    return TRUE;
}
static void DocumentProposalCancel(GtkButton *button, gpointer data)
{
    (void)button;
    DocumentProposalClose(GTK_WIDGET(data), 1, UMI_STATUS_CANCELLED);
}
static void DocumentProposalChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    GtkWidget *window = g_object_ref(data);
    DocumentProposalReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-proposal");
    if (DocumentProposalOwner(window, review) != NULL)
    {
        /* A nested text edit during comparison construction also invalidates
         * that comparison. Never enable Apply for bytes the user has not seen. */
        if (review->generation != UINT64_MAX)
            ++review->generation;
        review->presented_revision = 0U;
        gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
        gtk_widget_set_sensitive(review->apply, FALSE);
        DocumentProposalRemoveComparison(review);
        gtk_label_set_text(GTK_LABEL(review->message),
                           "Proposed text changed. Preview the complete draft again before approving.");
    }
    g_object_unref(window);
}
static void DocumentProposalPreview(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    DocumentProposalReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-proposal");
    UmiGtk4Adapter *owner = DocumentProposalOwner(window, review);
    if (owner == NULL || review->rendering)
    {
        g_object_unref(window);
        return;
    }
    review->rendering = 1;
    review->presented_revision = 0U;
    uint64_t generation = review->generation;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
    gtk_widget_set_sensitive(review->apply, FALSE);
    DocumentProposalRemoveComparison(review);
    owner = DocumentProposalOwner(window, review);
    UmiStatus status = owner == NULL ? UMI_STATUS_CANCELLED
                       : generation == UINT64_MAX
                           ? UMI_STATUS_CAPACITY_EXCEEDED
                           : UmiDocumentProposalCheck(owner->edit_coordinator, review->proposal);
    UmiDocumentProposalSummary summary = {0};
    if (status == UMI_STATUS_OK)
        status = UmiDocumentProposalInspect(review->proposal, &summary);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->input));
    if (status == UMI_STATUS_OK &&
        (size_t)gtk_text_buffer_get_char_count(buffer) > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK)
    {
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
        status = UmiDocumentProposalSetText(review->proposal, summary.revision, text, strlen(text));
        g_free(text);
    }
    const char *previous = NULL, *proposed = NULL;
    size_t previous_bytes = 0U, proposed_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentProposalTexts(review->proposal, &previous, &previous_bytes, &proposed,
                                          &proposed_bytes);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentProposalInspect(review->proposal, &summary);
    GtkWidget *comparison = NULL;
    /* The comparison owner copies both texts before creating GTK controls,
     * which may synchronously notify an observer that closes this window. */
    if (status == UMI_STATUS_OK)
        status = UmiGtk4TextComparisonCreate(previous, previous_bytes, proposed, proposed_bytes,
                                             "Captured source draft", "Proposed source draft", &comparison);
    if (comparison != NULL)
        g_object_ref_sink(comparison);
    if (DocumentProposalOwner(window, review) != NULL && review->generation == generation)
    {
        if (status == UMI_STATUS_OK)
        {
            gtk_box_append(GTK_BOX(review->comparison), comparison);
            if (DocumentProposalOwner(window, review) != NULL && review->generation == generation)
            {
                review->presented_revision = summary.revision;
                gtk_widget_set_sensitive(review->apply, TRUE);
                gtk_label_set_text(GTK_LABEL(review->message),
                                   summary.text_changes
                                       ? "Inspect both complete drafts, then approve and apply. Only the "
                                         "captured document changes; Undo restores it. Save remains separate."
                                       : "The proposed text is identical. Applying closes this review "
                                         "without creating an edit.");
            }
        }
        else
            gtk_label_set_text(GTK_LABEL(review->message),
                               status == UMI_STATUS_CAPACITY_EXCEEDED
                                   ? "The proposed draft exceeds the supported document or comparison limit. "
                                     "Reduce the proposed text."
                                   : "The captured source changed or the proposal could not be displayed. "
                                     "Close this window and capture the intended selection again.");
    }
    g_clear_object(&comparison);
    review->rendering = 0;
    g_object_unref(window);
}
static void DocumentProposalApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    DocumentProposalReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-proposal");
    UmiGtk4Adapter *owner = DocumentProposalOwner(window, review);
    if (owner != NULL && !review->rendering)
    {
        UmiStatus status =
            review->presented_revision == 0U                                  ? UMI_STATUS_INVALID_STATE
            : !gtk_check_button_get_active(GTK_CHECK_BUTTON(review->approve)) ? UMI_STATUS_PERMISSION_DENIED
            : owner->edit_cancel != NULL || UmiGtk4AdapterDocumentSaveAllBusy(owner) ||
                    UmiGtk4AdapterDocumentCloseBusy(owner) || UmiGtk4AdapterReplacementReviewBusy(owner)
                ? UMI_STATUS_BUSY
                : UmiDocumentProposalApply(owner->edit_coordinator, review->proposal,
                                           review->presented_revision, 1);
        if (status == UMI_STATUS_OK)
            DocumentProposalClose(window, 1, status);
        else
        {
            review->presented_revision = 0U;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
            gtk_widget_set_sensitive(review->apply, FALSE);
            gtk_label_set_text(
                GTK_LABEL(review->message),
                status == UMI_STATUS_PERMISSION_DENIED
                    ? "Preview the current proposed text, then explicitly approve it before applying."
                    : "The proposal is stale, not reviewed or another document operation is active. No "
                      "change was applied. Preview again, or capture a new selection if the source changed.");
        }
    }
    g_object_unref(window);
}
int UmiGtk4AdapterDocumentProposalBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
           g_object_get_data(adapter->edit_lifetime, "proposal-dialog") != NULL;
}
UmiStatus UmiGtk4AdapterCancelDocumentProposal(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiGtk4AdapterDocumentProposalBusy(adapter))
        DocumentProposalClose(g_object_get_data(adapter->edit_lifetime, "proposal-dialog"), 1,
                              UMI_STATUS_CANCELLED);
    return UMI_STATUS_OK;
}
static GtkWidget *DocumentProposalTextView(GtkWidget *box, const char *label, const char *id, int editable)
{
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_wrap(GTK_LABEL(caption), TRUE);
    gtk_box_append(GTK_BOX(box), caption);
    GtkWidget *view = g_object_ref_sink(gtk_text_view_new()), *scroll = gtk_scrolled_window_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_widget_set_size_request(scroll, -1, 110);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll);
    (void)umi_gtk4_automation_tag_widget(view, id);
    (void)UmiGtk4RecordingSetPrivate(view, 1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(view), GTK_ACCESSIBLE_PROPERTY_LABEL, label, -1);
    return view;
}
UmiStatus UmiGtk4AdapterReviewSelectedCode(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterDocumentProposalBusy(adapter) || UmiGtk4AdapterReplacementReviewBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        adapter->edit_cancel != NULL || g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentId id = 0U;
    UmiDocumentProposal *proposal = NULL;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentProposalCreate(adapter->edit_coordinator, id, &proposal);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentProposalSummary summary;
    const char *selected = NULL;
    size_t selected_bytes = 0U;
    status = UmiDocumentProposalInspect(proposal, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentProposalSelection(proposal, &selected, &selected_bytes);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentProposalDestroy(proposal);
        return status;
    }
    /* Retain identity before publishing a toplevel: construction observers may
     * destroy/rebind the host. No later construction dereferences adapter. */
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    DocumentProposalReview *review = g_new0(DocumentProposalReview, 1);
    review->lifetime = g_object_ref(lifetime);
    review->proposal = proposal;
    review->generation = 1U;
    g_object_set_data_full(G_OBJECT(window), "umicom-document-proposal", review, DocumentProposalReviewFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *scroll = gtk_scrolled_window_new();
    review->message =
        g_object_ref_sink(gtk_label_new("Paste or type the proposed replacement below, then Preview complete "
                                        "draft. No file or provider is accessed by this form."));
    gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
    gtk_box_append(GTK_BOX(box), review->message);
    char *target = g_strdup_printf("%s — captured selection: %zu bytes at byte %zu. %s", summary.display_name,
                                   summary.selection_bytes, summary.selection_offset,
                                   summary.has_unsaved_changes ? "Includes unsaved source edits." : "");
    GtkWidget *label = gtk_label_new(target);
    g_free(target);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(box), label);
    review->source =
        DocumentProposalTextView(box, "Captured selection (read only)", "document.proposal.source", 0);
    review->input = DocumentProposalTextView(box, "Proposed replacement (empty text deletes the selection)",
                                             "document.proposal.input", 1);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->source)), selected,
                             (gint)selected_bytes);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->input)), selected,
                             (gint)selected_bytes);
    review->preview = g_object_ref_sink(gtk_button_new_with_label("Preview complete draft"));
    gtk_box_append(GTK_BOX(box), review->preview);
    review->comparison = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_widget_set_vexpand(review->comparison, TRUE);
    (void)UmiGtk4RecordingSetPrivate(review->comparison, 1);
    gtk_box_append(GTK_BOX(box), review->comparison);
    review->approve = g_object_ref_sink(gtk_check_button_new_with_label(
        "I approve replacing this captured selection with the reviewed text"));
    gtk_box_append(GTK_BOX(box), review->approve);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    review->apply = g_object_ref_sink(gtk_button_new_with_label("Apply reviewed replacement"));
    gtk_widget_set_sensitive(review->apply, FALSE);
    review->cancel = g_object_ref_sink(gtk_button_new_with_label("Cancel"));
    gtk_box_append(GTK_BOX(buttons), review->apply);
    gtk_box_append(GTK_BOX(buttons), review->cancel);
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    gtk_window_set_title(GTK_WINDOW(window), "Review selected-code replacement");
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 800);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    (void)umi_gtk4_automation_tag_widget(window, "document.proposal.review");
    (void)umi_gtk4_automation_tag_widget(review->preview, "document.proposal.preview");
    (void)umi_gtk4_automation_tag_widget(review->apply, "document.proposal.apply");
    (void)umi_gtk4_automation_tag_widget(review->approve, "document.proposal.approve");
    (void)umi_gtk4_automation_tag_widget(review->cancel, "document.proposal.cancel");
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(review->input)), "changed",
                            G_CALLBACK(DocumentProposalChanged), window, 0);
    g_signal_connect_object(review->preview, "clicked", G_CALLBACK(DocumentProposalPreview), window, 0);
    g_signal_connect_object(review->apply, "clicked", G_CALLBACK(DocumentProposalApply), window, 0);
    g_signal_connect_object(review->cancel, "clicked", G_CALLBACK(DocumentProposalCancel), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(DocumentProposalClosing), NULL);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    else
    {
        g_object_set_data(lifetime, "proposal-dialog", window);
        status =
            UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), DocumentProposalRemoved, NULL);
        if (status == UMI_STATUS_OK)
            status =
                UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), DocumentProposalParentRemoved, window);
        if (status == UMI_STATUS_OK && review->closed)
            status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK)
    {
        gtk_window_set_default_widget(GTK_WINDOW(window), review->cancel);
        gtk_window_set_focus(GTK_WINDOW(window), review->input);
        /* Focus notifications may retire the host too. A closed modal must
         * not be presented again after its binding has been invalidated. */
        if (DocumentProposalOwner(window, review) != NULL)
            gtk_window_present(GTK_WINDOW(window));
        if (DocumentProposalOwner(window, review) == NULL)
            status = UMI_STATUS_CANCELLED;
    }
    else
        DocumentProposalClose(window, 0, status);
    g_object_unref(window);
    g_object_unref(parent);
    g_object_unref(lifetime);
    return status;
}
