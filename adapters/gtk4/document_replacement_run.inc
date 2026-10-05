/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_replacement_run.inc
 * PURPOSE: Present sequential replacement decisions through the shared GTK document binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_comparison.h"
#include "umicom/ui/gtk4/window_lifecycle.h"

/* The native window owns this state. The token borrows the current adapter;
 * retained controls never retain or rediscover a destroyed application owner. */
/* These labels describe the policy already validated by the document owner.
 * Keep them visible above a comparison so the user can identify its scope. */
static const char *ReplacementSearchPolicyLabel(const UmiEditorSearchOptions *options)
{
    if (options->case_mode == UMI_EDITOR_SEARCH_CASE_SENSITIVE)
        return options->whole_word ? "Search: Match case, whole word" : "Search: Match case";
    if (options->case_mode == UMI_EDITOR_SEARCH_CASE_ASCII_INSENSITIVE)
        return options->whole_word ? "Search: Ignore ASCII case, whole word" : "Search: Ignore ASCII case";
    return options->whole_word ? "Search: Smart case, whole word" : "Search: Smart case";
}

typedef struct ReplacementReview {
    GObject *lifetime;
    UmiDocumentReplacementSession *session;
    GtkWidget *message;
    GtkWidget *content;
    GtkWidget *apply;
    GtkWidget *skip;
    GtkWidget *stop;
    guint idle;
    int closed;
    int rendering;
    int presentation_failed;
} ReplacementReview;

static UmiGtk4Adapter *ReplacementReviewOwner(GtkWidget *window, ReplacementReview *review)
{
    if (review == NULL || review->closed ||
        g_object_get_data(review->lifetime, "replacement-dialog") != window) return NULL;
    UmiGtk4Adapter *adapter = g_object_get_data(review->lifetime, "adapter");
    return adapter != NULL && adapter->edit_coordinator != NULL && adapter->window != NULL &&
        UmiGtk4WindowIsOpen(adapter->window) ? adapter : NULL;
}

static void ReplacementReviewFree(gpointer data)
{
    ReplacementReview *review = data;
    UmiDocumentReplacementSessionDestroy(review->session);
    g_clear_object(&review->message);
    g_clear_object(&review->content);
    g_clear_object(&review->apply);
    g_clear_object(&review->skip);
    g_clear_object(&review->stop);
    g_clear_object(&review->lifetime);
    g_free(review);
}

/* Invalidate first. Removing an idle source releases its retained window and
 * may notify observers; no callback may then reach the borrowed coordinator. */
static void ReplacementReviewInvalidate(GtkWidget *window, ReplacementReview *review)
{
    review->closed = 1;
    if (g_object_get_data(review->lifetime, "replacement-dialog") == window)
        g_object_set_data(review->lifetime, "replacement-dialog", NULL);
    (void)UmiDocumentReplacementSessionCancel(review->session);
    if (review->idle != 0U) {
        guint idle = review->idle; review->idle = 0U;
        (void)g_source_remove(idle);
    }
}

/* Native removal also covers gtk_window_destroy when a test or host retains
 * the window. Waiting for GObject finalisation would leave live callbacks. */
static void ReplacementReviewRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    if (review == NULL || review->closed) return;
    GObject *lifetime = g_object_ref(review->lifetime);
    UmiGtk4Adapter *owner = ReplacementReviewOwner(window, review);
    ReplacementReviewInvalidate(window, review);
    UmiDocumentReplacementProgress progress;
    (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
    /* Notify last: completion may close the parent or replace the binding. */
    if (owner != NULL && g_object_get_data(lifetime, "adapter") == owner)
        (void)EditFinished(owner, review->presentation_failed ? UMI_STATUS_UNAVAILABLE : progress.last_status);
    g_object_unref(lifetime);
}

static void ReplacementReviewClose(GtkWidget *window, int notify)
{
    g_object_ref(window);
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    if (review != NULL && !review->closed) {
        if (notify) ReplacementReviewRemoved(window, NULL);
        else ReplacementReviewInvalidate(window, review);
    }
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
}

/* Parent removal is observable before finalisation when another host retains
 * it. Stop the sequence at that boundary without invoking the closing owner. */
static void ReplacementReviewParentRemoved(GtkWidget *parent, gpointer data)
{
    (void)parent;
    ReplacementReviewClose(GTK_WIDGET(data), 0);
}

/* The editing binding now retires both sequential and complete-set replacement windows before its coordinator ends.
 * The former implementation is retained for engineering review. */
#if 0
static void ReplacementReviewDetach(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "replacement-dialog");
    if (window != NULL) ReplacementReviewClose(window, 0);
}
#endif
/* Retire complete language-edit review before releasing the shared document lifetime binding.
 * The former implementation is retained for engineering review. */
#if 0
static void ReplacementReviewDetach(GObject *lifetime)
{
    ReplacementSetDetach(lifetime);
    GtkWidget *window = g_object_get_data(lifetime, "replacement-dialog");
    if (window != NULL) ReplacementReviewClose(window, 0);
}
#endif
static void ReplacementReviewDetach(GObject *lifetime)
{
    WorkspacePanelDetach(lifetime);
    ReplacementSetDetach(lifetime);
    GtkWidget *window = g_object_get_data(lifetime, "replacement-dialog");
    if (window != NULL) ReplacementReviewClose(window, 0);
}

static gboolean ReplacementReviewPump(gpointer data);
static void ReplacementReviewSchedule(GtkWidget *window, ReplacementReview *review)
{
    if (!review->closed && review->idle == 0U)
        review->idle = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, ReplacementReviewPump,
            g_object_ref(window), g_object_unref);
}

/* Widgets retain their own references so reentrant window teardown cannot
 * invalidate a pointer halfway through a label or sensitivity notification. */
static void ReplacementReviewRender(GtkWidget *window, ReplacementReview *review)
{
    if (ReplacementReviewOwner(window, review) == NULL || review->rendering) return;
    review->rendering = 1;
    UmiDocumentReplacementProgress progress;
    (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(review->content)) != NULL)
        gtk_box_remove(GTK_BOX(review->content), child);
    int asking = progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW;
    if (asking && !review->closed) {
        const char *previous = NULL, *proposed = NULL;
        size_t previous_bytes = 0U, proposed_bytes = 0U;
        GtkWidget *comparison = NULL;
        UmiStatus status = UmiDocumentReplacementSessionTexts(review->session,
            &previous, &previous_bytes, &proposed, &proposed_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4TextComparisonCreate(previous, previous_bytes, proposed, proposed_bytes,
                "Captured draft", "Proposed draft", &comparison);
        if (status != UMI_STATUS_OK) {
            review->presentation_failed = 1;
            (void)UmiDocumentReplacementSessionCancel(review->session);
            (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
            asking = 0;
        }
        if (comparison != NULL) {
            g_object_ref_sink(comparison);
            if (!review->closed) gtk_box_append(GTK_BOX(review->content), comparison);
            g_object_unref(comparison);
        }
    }
    if (!review->closed) {
        char *message = asking
            ? g_strdup_printf("Review %zu replacements in %s. %zu of %zu documents remain.\n"
                "Apply changes only this draft; Skip leaves it alone. Stop keeps earlier edits.\n"
                "Use Undo in each changed document to restore its draft. Save is separate.",
                progress.current.match_count, progress.current.display_name, progress.remaining, progress.total)
            : g_strdup_printf("%s\nChanged: %zu documents, %zu replacements. Skipped: %zu. "
                "Unchanged: %zu. Read-only: %zu. Closed: %zu. Remaining: %zu.\n"
                "No files were saved. Earlier accepted edits remain in their drafts.",
                review->presentation_failed ? "The comparison could not be displayed. No unreviewed draft was applied." :
                progress.phase == UMI_DOCUMENT_REPLACEMENT_READY ? "Inspecting open documents..." :
                progress.phase == UMI_DOCUMENT_REPLACEMENT_COMPLETE ? "Review complete." :
                progress.phase == UMI_DOCUMENT_REPLACEMENT_CANCELLED ? "Review stopped." :
                "Review stopped because a document changed or became unavailable. Start a fresh review.",
                progress.applied, progress.replacements, progress.skipped, progress.unchanged,
                progress.read_only, progress.closed, progress.remaining);
        gtk_label_set_text(GTK_LABEL(review->message), message); g_free(message);
        gtk_widget_set_sensitive(review->apply, asking);
        gtk_widget_set_sensitive(review->skip, asking);
        gtk_button_set_label(GTK_BUTTON(review->stop),
            progress.phase == UMI_DOCUMENT_REPLACEMENT_READY || asking ? "Stop" : "Close");
    }
    review->rendering = 0;
}

static gboolean ReplacementReviewPump(gpointer data)
{
    GtkWidget *window = data;
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    review->idle = 0U;
    if (ReplacementReviewOwner(window, review) == NULL) return G_SOURCE_REMOVE;
    (void)UmiDocumentReplacementSessionStep(review->session);
    ReplacementReviewRender(window, review);
    if (ReplacementReviewOwner(window, review) != NULL) {
        UmiDocumentReplacementProgress progress;
        (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
        if (progress.phase == UMI_DOCUMENT_REPLACEMENT_READY) ReplacementReviewSchedule(window, review);
    }
    return G_SOURCE_REMOVE;
}

static void ReplacementReviewRespond(GtkButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    if (ReplacementReviewOwner(window, review) != NULL && !review->rendering) {
        UmiDocumentReplacementProgress progress;
        (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
        if (GTK_WIDGET(button) == review->stop) {
            if (progress.phase == UMI_DOCUMENT_REPLACEMENT_READY || progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW) {
                (void)UmiDocumentReplacementSessionCancel(review->session);
                if (review->idle != 0U) { guint idle = review->idle; review->idle = 0U; (void)g_source_remove(idle); }
                ReplacementReviewRender(window, review);
            } else ReplacementReviewClose(window, 1);
        } else if (progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW) {
            (void)UmiDocumentReplacementSessionRespond(review->session,
                GTK_WIDGET(button) == review->apply ? UMI_DOCUMENT_REPLACEMENT_APPLY : UMI_DOCUMENT_REPLACEMENT_SKIP);
            ReplacementReviewRender(window, review);
            (void)UmiDocumentReplacementSessionProgress(review->session, &progress);
            if (progress.phase == UMI_DOCUMENT_REPLACEMENT_READY) ReplacementReviewSchedule(window, review);
        }
    }
    g_object_unref(window);
}

static gboolean ReplacementReviewClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    ReplacementReviewClose(GTK_WIDGET(window), 1);
    return TRUE;
}

/* Shared busy guards cover both replacement workflows; the earlier predicate
 * remains for engineering review. */
#if 0
int UmiGtk4AdapterReplacementReviewBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        g_object_get_data(adapter->edit_lifetime, "replacement-dialog") != NULL;
}
#endif
/* Creation notifications must also respect the replacement busy boundary.
 * The earlier predicate is retained for engineering review. */
#if 0
int UmiGtk4AdapterReplacementReviewBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        (g_object_get_data(adapter->edit_lifetime, "replacement-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL);
}
#endif
/* Language workspace review shares the document transaction busy boundary.
 * The earlier replacement-only predicate is retained for review. */
#if 0
int UmiGtk4AdapterReplacementReviewBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        (g_object_get_data(adapter->edit_lifetime, "replacement-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "replacement-set-creating") != NULL);
}
#endif
int UmiGtk4AdapterReplacementReviewBusy(const UmiGtk4Adapter *adapter)
{
    return adapter != NULL && adapter->edit_lifetime != NULL &&
        (g_object_get_data(adapter->edit_lifetime, "replacement-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "replacement-set-creating") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "workspace-edit-dialog") != NULL ||
         g_object_get_data(adapter->edit_lifetime, "workspace-edit-creating") != NULL);
}

/* The sequential progress API refuses a different review model instead of interpreting its window as a sequential session.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReplacementReviewProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentReplacementProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_NOT_FOUND;
    GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    return UmiDocumentReplacementSessionProgress(review->session, outProgress);
}
#endif
/* A busy creation reservation has no sequential window to inspect yet.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReplacementReviewProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentReplacementProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_NOT_FOUND;
    if (g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL)
        return UMI_STATUS_NOT_IMPLEMENTED; /* Sequential progress does not describe an atomic set. */
    GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    return UmiDocumentReplacementSessionProgress(review->session, outProgress);
}
#endif
/* Sequential progress cannot represent a complete language workspace edit; callers use its dedicated model.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReplacementReviewProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentReplacementProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_NOT_FOUND;
    if (g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL)
        return UMI_STATUS_NOT_IMPLEMENTED; /* Sequential progress does not describe an atomic set. */
    GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
    if (window == NULL) return UMI_STATUS_BUSY;
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    return UmiDocumentReplacementSessionProgress(review->session, outProgress);
}
#endif
UmiStatus UmiGtk4AdapterReplacementReviewProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentReplacementProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_NOT_FOUND;
    if (g_object_get_data(adapter->edit_lifetime, "replacement-set-dialog") != NULL ||
        g_object_get_data(adapter->edit_lifetime, "workspace-edit-dialog") != NULL)
        return UMI_STATUS_NOT_IMPLEMENTED; /* Sequential progress does not describe an atomic set. */
    GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
    if (window == NULL) return UMI_STATUS_BUSY;
    ReplacementReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-review");
    return UmiDocumentReplacementSessionProgress(review->session, outProgress);
}

/* The public cancellation command closes either replacement workflow through the same lifetime boundary.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterCancelReplacementReview(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter)) {
        GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
        ReplacementReviewClose(window, 1);
    }
    return UMI_STATUS_OK;
}
#endif
/* Cancellation during native construction reports busy until there is a window to retire.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterCancelReplacementReview(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime != NULL) ReplacementSetDetach(adapter->edit_lifetime);
    if (UmiGtk4AdapterReplacementReviewBusy(adapter)) {
        GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
        ReplacementReviewClose(window, 1);
    }
    return UMI_STATUS_OK;
}
#endif
/* Cancel complete language review through its owning window, including retained controls.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterCancelReplacementReview(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime != NULL) ReplacementSetDetach(adapter->edit_lifetime);
    if (UmiGtk4AdapterReplacementReviewBusy(adapter)) {
        GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
        if (window == NULL) return UMI_STATUS_BUSY;
        ReplacementReviewClose(window, 1);
    }
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiGtk4AdapterCancelReplacementReview(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime != NULL) { WorkspacePanelDetach(adapter->edit_lifetime); ReplacementSetDetach(adapter->edit_lifetime); }
    if (UmiGtk4AdapterReplacementReviewBusy(adapter)) {
        GtkWidget *window = g_object_get_data(adapter->edit_lifetime, "replacement-dialog");
        if (window == NULL) return UMI_STATUS_BUSY;
        ReplacementReviewClose(window, 1);
    }
    return UMI_STATUS_OK;
}

/* Native replacement review now captures and displays explicit case and word policy; its original lifetime and approval flow remain shared.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReviewOpenReplacements(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL) return UMI_STATUS_BUSY;
    UmiDocumentReplacementSession *session = NULL;
    UmiStatus status = UmiDocumentReplacementSessionCreate(adapter->edit_coordinator, needle, replacement, &session);
    if (status != UMI_STATUS_OK) return status;
    /* Creating a toplevel publishes it to GTK observers. Retain the parent and
     * identity token first; an observer may tear down the application owner. */
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    ReplacementReview *review = g_new0(ReplacementReview, 1);
    /* Use the pre-notification identity rather than dereferencing a potentially destroyed adapter. The former expression is retained for review. */
#if 0
    review->lifetime = g_object_ref(adapter->edit_lifetime);
#endif
    review->lifetime = g_object_ref(lifetime);
    review->session = session;
    g_object_set_data_full(G_OBJECT(window), "umicom-replacement-review", review, ReplacementReviewFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    review->message = g_object_ref_sink(gtk_label_new("Inspecting open documents..."));
    review->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    review->apply = g_object_ref_sink(gtk_button_new_with_label("Apply to this document"));
    review->skip = g_object_ref_sink(gtk_button_new_with_label("Skip this document"));
    review->stop = g_object_ref_sink(gtk_button_new_with_label("Stop"));
    gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(review->message), 0.0F);
    gtk_widget_set_vexpand(review->content, TRUE);
    gtk_widget_set_sensitive(review->apply, FALSE);
    gtk_widget_set_sensitive(review->skip, FALSE);
    gtk_box_append(GTK_BOX(box), review->message);
    gtk_box_append(GTK_BOX(box), review->content);
    gtk_box_append(GTK_BOX(buttons), review->apply);
    gtk_box_append(GTK_BOX(buttons), review->skip);
    gtk_box_append(GTK_BOX(buttons), review->stop);
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_title(GTK_WINDOW(window), "Review replacements in open documents");
    gtk_window_set_default_size(GTK_WINDOW(window), 1050, 720);
    /* Use the retained parent across native widget notifications. The former expression is retained for review. */
#if 0
    gtk_window_set_transient_for(GTK_WINDOW(window), adapter->window);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    (void)umi_gtk4_automation_tag_widget(window, "document.replacements.review");
    (void)umi_gtk4_automation_tag_widget(review->message, "document.replacements.summary");
    (void)umi_gtk4_automation_tag_widget(review->apply, "document.replacements.apply");
    (void)umi_gtk4_automation_tag_widget(review->skip, "document.replacements.skip");
    (void)umi_gtk4_automation_tag_widget(review->stop, "document.replacements.stop");
    g_signal_connect_object(review->apply, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect_object(review->skip, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect_object(review->stop, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(ReplacementReviewClosing), NULL);
    /* Publish only on the verified original token, never a later replacement binding. The former expression is retained for review. */
#if 0
    g_object_set_data(adapter->edit_lifetime, "replacement-dialog", window);
#endif
    /* The adapter address is compared, not dereferenced, after GTK callbacks. */
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) {
        ReplacementReviewClose(window, 0);
        g_object_unref(parent); g_object_unref(lifetime); g_object_unref(window);
        return UMI_STATUS_CANCELLED;
    }
    g_object_set_data(lifetime, "replacement-dialog", window);
    status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), ReplacementReviewRemoved, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), ReplacementReviewParentRemoved, window);
    /* Either watch may synchronously discover an already-closed window. */
    if (status == UMI_STATUS_OK && review->closed) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) {
        ReplacementReviewSchedule(window, review);
        /* Keyboard activation starts on the non-mutating choice. */
        gtk_window_set_default_widget(GTK_WINDOW(window), review->stop);
        gtk_window_set_focus(GTK_WINDOW(window), review->stop);
        gtk_window_present(GTK_WINDOW(window));
    } else ReplacementReviewClose(window, 0);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
#endif
UmiStatus UmiGtk4AdapterReviewOpenReplacementsWithOptions(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement, const UmiEditorSearchOptions *searchOptions)
{
    /* Copy before GTK publishes a new toplevel and notifies native observers. */
    UmiEditorSearchOptions capturedOptions = searchOptions != NULL ? *searchOptions :
        (UmiEditorSearchOptions){UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0U};

    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || !UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL) return UMI_STATUS_BUSY;
    UmiDocumentReplacementSession *session = NULL;
    UmiStatus status = UmiDocumentReplacementSessionCreateWithOptions(adapter->edit_coordinator, needle, replacement, &capturedOptions, &session);
    if (status != UMI_STATUS_OK) return status;
    /* Creating a toplevel publishes it to GTK observers. Retain the parent and
     * identity token first; an observer may tear down the application owner. */
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    ReplacementReview *review = g_new0(ReplacementReview, 1);
    /* Use the pre-notification identity rather than dereferencing a potentially destroyed adapter. The former expression is retained for review. */
#if 0
    review->lifetime = g_object_ref(adapter->edit_lifetime);
#endif
    review->lifetime = g_object_ref(lifetime);
    review->session = session;
    g_object_set_data_full(G_OBJECT(window), "umicom-replacement-review", review, ReplacementReviewFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    review->message = g_object_ref_sink(gtk_label_new("Inspecting open documents..."));
    review->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    review->apply = g_object_ref_sink(gtk_button_new_with_label("Apply to this document"));
    review->skip = g_object_ref_sink(gtk_button_new_with_label("Skip this document"));
    review->stop = g_object_ref_sink(gtk_button_new_with_label("Stop"));
    gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(review->message), 0.0F);
    gtk_widget_set_vexpand(review->content, TRUE);
    gtk_widget_set_sensitive(review->apply, FALSE);
    gtk_widget_set_sensitive(review->skip, FALSE);
    GtkWidget *policyLabel = gtk_label_new(ReplacementSearchPolicyLabel(&capturedOptions));
    gtk_label_set_xalign(GTK_LABEL(policyLabel), 0.0F);
    (void)umi_gtk4_automation_tag_widget(policyLabel, "document.replacements.search-policy");
    gtk_box_append(GTK_BOX(box), policyLabel);
    gtk_box_append(GTK_BOX(box), review->message);
    gtk_box_append(GTK_BOX(box), review->content);
    gtk_box_append(GTK_BOX(buttons), review->apply);
    gtk_box_append(GTK_BOX(buttons), review->skip);
    gtk_box_append(GTK_BOX(buttons), review->stop);
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_title(GTK_WINDOW(window), "Review replacements in open documents");
    gtk_window_set_default_size(GTK_WINDOW(window), 1050, 720);
    /* Use the retained parent across native widget notifications. The former expression is retained for review. */
#if 0
    gtk_window_set_transient_for(GTK_WINDOW(window), adapter->window);
#endif
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    (void)umi_gtk4_automation_tag_widget(window, "document.replacements.review");
    (void)umi_gtk4_automation_tag_widget(review->message, "document.replacements.summary");
    (void)umi_gtk4_automation_tag_widget(review->apply, "document.replacements.apply");
    (void)umi_gtk4_automation_tag_widget(review->skip, "document.replacements.skip");
    (void)umi_gtk4_automation_tag_widget(review->stop, "document.replacements.stop");
    g_signal_connect_object(review->apply, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect_object(review->skip, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect_object(review->stop, "clicked", G_CALLBACK(ReplacementReviewRespond), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(ReplacementReviewClosing), NULL);
    /* Publish only on the verified original token, never a later replacement binding. The former expression is retained for review. */
#if 0
    g_object_set_data(adapter->edit_lifetime, "replacement-dialog", window);
#endif
    /* The adapter address is compared, not dereferenced, after GTK callbacks. */
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) {
        ReplacementReviewClose(window, 0);
        g_object_unref(parent); g_object_unref(lifetime); g_object_unref(window);
        return UMI_STATUS_CANCELLED;
    }
    g_object_set_data(lifetime, "replacement-dialog", window);
    status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), ReplacementReviewRemoved, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), ReplacementReviewParentRemoved, window);
    /* Either watch may synchronously discover an already-closed window. */
    if (status == UMI_STATUS_OK && review->closed) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) {
        ReplacementReviewSchedule(window, review);
        /* Keyboard activation starts on the non-mutating choice. */
        gtk_window_set_default_widget(GTK_WINDOW(window), review->stop);
        gtk_window_set_focus(GTK_WINDOW(window), review->stop);
        gtk_window_present(GTK_WINDOW(window));
    } else ReplacementReviewClose(window, 0);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
UmiStatus UmiGtk4AdapterReviewOpenReplacements(UmiGtk4Adapter *adapter, const char *needle, const char *replacement)
{
    return UmiGtk4AdapterReviewOpenReplacementsWithOptions(adapter, needle, replacement, NULL);
}

