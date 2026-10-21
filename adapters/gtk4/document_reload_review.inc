/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_reload_review.inc
 * PURPOSE: Review saved-file changes through the document owner's reload plan.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_comparison.h"

typedef struct DocumentReloadReview {
    GObject *lifetime;
    UmiDocumentReloadPlan *plan;
    GtkWidget *content, *approve, *apply, *cancel, *message;
    int closed, ready, applying, notify, notified;
    UmiStatus result;
} DocumentReloadReview;

/* Authority belongs to the editing binding, not a retained native control.
 * Native removal is checked separately from final GObject destruction. */
static UmiGtk4Adapter *ReloadReviewOwner(GtkWidget *window, DocumentReloadReview *review)
{
    if (review == NULL || review->closed ||
        g_object_get_data(review->lifetime, "reload-dialog") != window ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(review->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
        UmiGtk4WindowIsOpen(owner->window) ? owner : NULL;
}
static void ReloadReviewFree(gpointer data)
{
    DocumentReloadReview *review = data;
    UmiDocumentReloadPlanDestroy(review->plan);
    g_clear_object(&review->content);
    g_clear_object(&review->lifetime);
    g_free(review);
}
static void ReloadReviewNotify(DocumentReloadReview *review)
{
    if (!review->ready || !review->notify || review->notified || review->applying) return;
    review->notified = 1;
    UmiGtk4Adapter *owner = g_object_get_data(review->lifetime, "adapter");
    if (owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
        UmiGtk4WindowIsOpen(owner->window)) (void)EditFinished(owner, review->result);
}
static void ReloadReviewClose(GtkWidget *window, int notify, UmiStatus result)
{
    g_object_ref(window);
    DocumentReloadReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-reload");
    if (review != NULL && !review->closed) {
        review->closed = 1;
        review->notify = notify;
        review->result = result;
        if (g_object_get_data(review->lifetime, "reload-dialog") == window)
            g_object_set_data(review->lifetime, "reload-dialog", NULL);
        /* A provider callback may close this form during Apply. Keep its plan
         * alive until the document transaction has returned to this caller. */
        if (!review->applying) {
            UmiDocumentReloadPlanDestroy(review->plan);
            review->plan = NULL;
        }
        gtk_window_destroy(GTK_WINDOW(window));
        ReloadReviewNotify(review);
    }
    g_object_unref(window);
}
static void ReloadReviewRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    ReloadReviewClose(window, 1, UMI_STATUS_CANCELLED);
}
static void ReloadReviewDetach(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "reload-dialog");
    if (window != NULL) ReloadReviewClose(window, 0, UMI_STATUS_CANCELLED);
}
static gboolean ReloadReviewClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    ReloadReviewClose(GTK_WIDGET(window), 1, UMI_STATUS_CANCELLED);
    return TRUE;
}
static int ReloadReviewControl(GtkWidget *window, GtkWidget *control, DocumentReloadReview *review)
{
    return ReloadReviewOwner(window, review) != NULL && review->ready && !review->applying &&
        gtk_widget_get_visible(window) && gtk_widget_get_root(control) == GTK_ROOT(window);
}
static void ReloadReviewCancel(GtkButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    DocumentReloadReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-reload");
    if (ReloadReviewControl(window, GTK_WIDGET(button), review))
        ReloadReviewClose(window, 1, UMI_STATUS_CANCELLED);
    g_object_unref(window);
}
static void ReloadReviewApproval(GtkCheckButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    DocumentReloadReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-reload");
    if (ReloadReviewControl(window, GTK_WIDGET(button), review))
        gtk_widget_set_sensitive(review->apply, gtk_check_button_get_active(button));
    g_object_unref(window);
}
static void ReloadReviewApply(GtkButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    DocumentReloadReview *review = g_object_get_data(G_OBJECT(window), "umicom-document-reload");
    if (!ReloadReviewControl(window, GTK_WIDGET(button), review) ||
        gtk_widget_get_root(review->approve) != GTK_ROOT(window) ||
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(review->approve))) {
        g_object_unref(window);
        return;
    }
    UmiGtk4Adapter *owner = ReloadReviewOwner(window, review);
    review->applying = 1;
    UmiStatus status = UmiDocumentCoordinatorApplyReload(owner->edit_coordinator, review->plan, 1);
    review->applying = 0;
    if (review->closed) {
        UmiDocumentReloadPlanDestroy(review->plan);
        review->plan = NULL;
        /* If Apply completed while a provider callback closed the window,
         * report the actual transaction result instead of claiming cancellation. */
        review->result = status;
        ReloadReviewNotify(review);
    } else if (ReloadReviewOwner(window, review) == NULL) {
        ReloadReviewClose(window, 0, UMI_STATUS_CANCELLED);
    } else if (status == UMI_STATUS_OK) {
        ReloadReviewClose(window, 1, status);
    } else {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(review->approve), FALSE);
        if (ReloadReviewOwner(window, review) != NULL)
            gtk_widget_set_sensitive(review->apply, FALSE);
        if (ReloadReviewOwner(window, review) != NULL)
            gtk_label_set_text(GTK_LABEL(review->message),
                "Reload was not applied. The draft, saved file or its permissions may have changed. "
                "Keep the current draft and open a new comparison before trying again.");
    }
    g_object_unref(window);
}
int UmiGtk4AdapterDocumentReloadBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        (g_object_get_data(adapter->edit_lifetime, "reload-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "reload-creating") != NULL);
}
UmiStatus UmiGtk4AdapterCancelDocumentReload(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *window = adapter->edit_lifetime != NULL
        ? g_object_get_data(adapter->edit_lifetime, "reload-dialog") : NULL;
    if (window != NULL) ReloadReviewClose(window, 1, UMI_STATUS_CANCELLED);
    else if (UmiGtk4AdapterDocumentReloadBusy(adapter)) return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}
UmiStatus UmiGtk4AdapterReviewDocumentReload(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter) ||
        UmiGtk4AdapterCompletionReviewBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-constructing") != NULL) return UMI_STATUS_BUSY;
    UmiDocumentId id = 0U;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status != UMI_STATUS_OK) return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    UmiDocumentReloadPlan *plan = NULL;
    /* Reserve before provider reads and GTK creation: nested invocations must
     * not capture a second question against the same document binding. */
    g_object_set_data(lifetime, "reload-creating", lifetime);
    status = UmiDocumentCoordinatorPrepareReload(adapter->edit_coordinator, id, &plan);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent))
        status = UMI_STATUS_CANCELLED;
    UmiDocumentReloadSummary summary;
    const char *previous = NULL, *incoming = NULL;
    size_t previousLength = 0U, incomingLength = 0U;
    if (status == UMI_STATUS_OK) status = UmiDocumentReloadPlanSummary(plan, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentReloadPlanTexts(plan, &previous, &previousLength, &incoming, &incomingLength);
    if (status != UMI_STATUS_OK) {
        UmiDocumentReloadPlanDestroy(plan);
        g_object_set_data(lifetime, "reload-creating", NULL);
        g_object_unref(parent);
        g_object_unref(lifetime);
        return status;
    }
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    DocumentReloadReview *review = g_new0(DocumentReloadReview, 1);
    review->lifetime = g_object_ref(lifetime);
    review->plan = plan;
    g_object_set_data_full(G_OBJECT(window), "umicom-document-reload", review, ReloadReviewFree);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) status = UMI_STATUS_CANCELLED;
    GtkWidget *comparison = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiGtk4TextComparisonCreate(previous, previousLength, incoming, incomingLength,
            "Current editor draft", "Saved file on disk", &comparison);
    if (comparison != NULL) g_object_ref_sink(comparison);
    if (status == UMI_STATUS_OK) {
        review->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
        GtkWidget *caption = gtk_label_new(summary.display_name);
        gtk_label_set_selectable(GTK_LABEL(caption), TRUE);
        gtk_box_append(GTK_BOX(review->content), caption);
        gtk_widget_set_vexpand(comparison, TRUE);
        gtk_box_append(GTK_BOX(review->content), comparison);
        char description[1024];
        (void)snprintf(description, sizeof description,
            "%s\nCurrent text: %zu bytes. Saved text: %zu bytes. Encoding: %s; line endings: %s. "
            "Both panes show decoded UTF-8 with normalised line endings. Reload does not write the file. "
            "Undo can restore the replaced text while the document remains open.",
            summary.has_unsaved_changes
                ? "This document has unsaved changes. Keep Current Draft or use Save As to preserve a separate copy."
                : "Review the saved file before adopting it into this document.",
            summary.previous_bytes, summary.incoming_bytes,
            umi_document_encoding_text(summary.incoming_encoding),
            umi_document_line_ending_text(summary.incoming_line_ending));
        review->message = gtk_label_new(description);
        gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
        gtk_box_append(GTK_BOX(review->content), review->message);
        review->approve = gtk_check_button_new_with_label(
            "I reviewed both texts and approve replacing this draft with the displayed saved file");
        gtk_box_append(GTK_BOX(review->content), review->approve);
        GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        review->cancel = gtk_button_new_with_label("Keep Current Draft");
        review->apply = gtk_button_new_with_label("Reload Reviewed File");
        gtk_box_append(GTK_BOX(buttons), review->cancel);
        gtk_box_append(GTK_BOX(buttons), review->apply);
        gtk_box_append(GTK_BOX(review->content), buttons);
        gtk_widget_set_sensitive(review->apply, FALSE);
        gtk_window_set_child(GTK_WINDOW(window), review->content);
        gtk_window_set_title(GTK_WINDOW(window), "Review File Reload");
        gtk_window_set_default_size(GTK_WINDOW(window), 980, 640);
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_modal(GTK_WINDOW(window), TRUE);
        (void)umi_gtk4_automation_tag_widget(window, "document.reload.window");
        (void)umi_gtk4_automation_tag_widget(review->approve, "document.reload.approve");
        (void)umi_gtk4_automation_tag_widget(review->apply, "document.reload.apply");
        (void)umi_gtk4_automation_tag_widget(review->cancel, "document.reload.cancel");
        (void)umi_gtk4_automation_tag_widget(review->message, "document.reload.message");
        g_signal_connect(window, "close-request", G_CALLBACK(ReloadReviewClosing), NULL);
        g_signal_connect_object(review->approve, "toggled", G_CALLBACK(ReloadReviewApproval), G_OBJECT(window), 0);
        g_signal_connect_object(review->apply, "clicked", G_CALLBACK(ReloadReviewApply), G_OBJECT(window), 0);
        g_signal_connect_object(review->cancel, "clicked", G_CALLBACK(ReloadReviewCancel), G_OBJECT(window), 0);
    }
    /* All widgets remain owned while construction observers run. Publish the
     * decision only if its original editor binding and native parent survive. */
    if (status == UMI_STATUS_OK && (g_object_get_data(lifetime, "adapter") != adapter ||
        !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(GTK_WINDOW(window))))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) {
        g_object_set_data(lifetime, "reload-dialog", window);
        status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), ReloadReviewRemoved, NULL);
        if (status == UMI_STATUS_OK && ReloadReviewOwner(window, review) == NULL)
            status = UMI_STATUS_CANCELLED;
        if (status == UMI_STATUS_OK) {
            review->ready = 1;
            gtk_window_present(GTK_WINDOW(window));
            if (ReloadReviewOwner(window, review) != NULL)
                (void)gtk_widget_grab_focus(review->cancel);
            if (ReloadReviewOwner(window, review) == NULL) status = UMI_STATUS_CANCELLED;
        }
    }
    g_object_set_data(lifetime, "reload-creating", NULL);
    if (status != UMI_STATUS_OK) ReloadReviewClose(window, 0, status);
    g_clear_object(&comparison);
    g_object_unref(window);
    g_object_unref(parent);
    g_object_unref(lifetime);
    return status;
}
