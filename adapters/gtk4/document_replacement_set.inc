/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_replacement_set.inc
 * PURPOSE: Present complete open-draft replacement review before one atomic application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/replacement_set.h"

typedef struct ReplacementSetReview
{
    GObject *lifetime;
    UmiDocumentReplacementSet *set;
    GtkWidget *message, *content, *previous, *next, *mark, *approval, *apply, *close;
    size_t selected, presented;
    uint64_t presented_revision;
    UmiStatus last_status;
    int closed, rendering;
} ReplacementSetReview;

static UmiGtk4Adapter *ReplacementSetOwner(GtkWidget *window, ReplacementSetReview *review)
{
    if (review == NULL || review->closed ||
        g_object_get_data(review->lifetime, "replacement-set-dialog") != window)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(review->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void ReplacementSetFree(gpointer data)
{
    ReplacementSetReview *review = data;
    UmiDocumentReplacementSetDestroy(review->set);
    g_clear_object(&review->message);
    g_clear_object(&review->content);
    g_clear_object(&review->previous);
    g_clear_object(&review->next);
    g_clear_object(&review->mark);
    g_clear_object(&review->approval);
    g_clear_object(&review->apply);
    g_clear_object(&review->close);
    g_clear_object(&review->lifetime);
    g_free(review);
}
/* Invalidation precedes native destruction. A retained button can keep a
 * widget alive, but it cannot reacquire a retired document coordinator. */
static void ReplacementSetInvalidate(GtkWidget *window, ReplacementSetReview *review)
{
    review->closed = 1;
    review->presented = SIZE_MAX;
    review->presented_revision = 0U;
    if (g_object_get_data(review->lifetime, "replacement-set-dialog") == window)
        g_object_set_data(review->lifetime, "replacement-set-dialog", NULL);
}
static void ReplacementSetRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    ReplacementSetReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-set");
    if (review != NULL && !review->closed)
        ReplacementSetInvalidate(window, review);
}
static void ReplacementSetClose(GtkWidget *window)
{
    g_object_ref(window);
    ReplacementSetRemoved(window, NULL);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
}
static void ReplacementSetParentRemoved(GtkWidget *parent, gpointer data)
{
    (void)parent;
    ReplacementSetClose(GTK_WIDGET(data));
}
static void ReplacementSetDetach(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "replacement-set-dialog");
    if (window != NULL)
        ReplacementSetClose(window);
}
static gboolean ReplacementSetClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    ReplacementSetClose(GTK_WIDGET(window));
    return TRUE;
}

/* All borrowed source is copied into the comparison before presentation.
 * Retained widget references and owner checks tolerate synchronous GTK
 * observers which close either window while labels or children change. */
static void ReplacementSetRender(GtkWidget *window, ReplacementSetReview *review)
{
    if (ReplacementSetOwner(window, review) == NULL || review->rendering)
        return;
    review->rendering = 1;
    review->presented = SIZE_MAX;
    review->presented_revision = 0U;
    UmiDocumentReplacementSetSummary summary;
    UmiStatus status = UmiDocumentReplacementSetInspect(review->set, &summary);
    UmiDocumentSourceRequestSummary document = {0};
    size_t matches = 0U;
    int reviewed = 0;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentReplacementSetAt(review->set, review->selected, &document, &matches, &reviewed);
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(review->content)) != NULL)
        gtk_box_remove(GTK_BOX(review->content), child);
    GtkWidget *comparison = NULL;
    if (status == UMI_STATUS_OK && !summary.applied && !review->closed)
    {
        const char *before = NULL, *after = NULL;
        size_t before_bytes = 0U, after_bytes = 0U;
        status = UmiDocumentReplacementSetTexts(review->set, review->selected, &before, &before_bytes, &after,
                                                &after_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4TextComparisonCreate(before, before_bytes, after, after_bytes, "Captured draft",
                                                 "Proposed draft", &comparison);
        if (comparison != NULL)
        {
            g_object_ref_sink(comparison);
            if (ReplacementSetOwner(window, review) != NULL)
                gtk_box_append(GTK_BOX(review->content), comparison);
            g_object_unref(comparison);
        }
        if (status == UMI_STATUS_OK && comparison != NULL && ReplacementSetOwner(window, review) != NULL)
        {
            review->presented = review->selected;
            review->presented_revision = summary.revision;
        }
    }
    if (status != UMI_STATUS_OK)
        review->last_status = status;
    if (!review->closed && status == UMI_STATUS_OK)
    {
        char *message =
            summary.applied
                ? g_strdup_printf("Applied %zu changed drafts. No files were saved. Use Undo separately in "
                                  "each changed document.",
                                  summary.changed_count)
                : g_strdup_printf(
                      "Document %zu of %zu: %s\n%zu matches here. %zu of %zu changed drafts reviewed. %s\n"
                      "Review every changed comparison, then approve and apply the complete set. Save is "
                      "separate.%s%s",
                      review->selected + 1U, summary.document_count, document.display_name, matches,
                      summary.reviewed_count, summary.changed_count,
                      document.text_changes ? (reviewed ? "This draft is reviewed." : "This draft changes.")
                                            : "This draft is unchanged.",
                      review->last_status == UMI_STATUS_OK ? "" : "\nLast action: ",
                      review->last_status == UMI_STATUS_OK ? "" : umi_status_text(review->last_status));
        gtk_label_set_text(GTK_LABEL(review->message), message);
        g_free(message);
        int ready = !summary.applied && review->last_status == UMI_STATUS_OK;
        gtk_widget_set_sensitive(review->previous, ready && review->selected > 0U);
        gtk_widget_set_sensitive(review->next, ready && review->selected + 1U < summary.document_count);
        gtk_widget_set_sensitive(review->mark, ready && review->presented == review->selected && !reviewed &&
                                                   document.text_changes);
        int all_reviewed =
            ready && summary.changed_count != 0U && summary.reviewed_count == summary.changed_count;
        gtk_widget_set_sensitive(review->approval, all_reviewed);
        gtk_widget_set_sensitive(
            review->apply, all_reviewed && gtk_check_button_get_active(GTK_CHECK_BUTTON(review->approval)));
    }
    else if (!review->closed)
    {
        gtk_label_set_text(GTK_LABEL(review->message),
                           "The full comparison could not be prepared. Close this review and start again. No "
                           "drafts were changed.");
        gtk_widget_set_sensitive(review->apply, FALSE);
        gtk_widget_set_sensitive(review->mark, FALSE);
        gtk_widget_set_sensitive(review->approval, FALSE);
    }
    review->rendering = 0;
}
static void ReplacementSetApprovalChanged(GtkCheckButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    ReplacementSetReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-set");
    if (ReplacementSetOwner(window, review) != NULL && !review->rendering)
        ReplacementSetRender(window, review);
    g_object_unref(window);
}
/* A failed comparison cannot be bypassed by activating a retained Apply control; only a currently presented review can commit.
 * The former implementation is retained for engineering review. */
#if 0
static void ReplacementSetRespond(GtkButton *button,gpointer data)
{
    GtkWidget *window=g_object_ref(data);ReplacementSetReview *review=g_object_get_data(G_OBJECT(window),"umicom-replacement-set");
    UmiGtk4Adapter *owner=ReplacementSetOwner(window,review);
    if(owner!=NULL && !review->rendering) {
        if(GTK_WIDGET(button)==review->close) ReplacementSetClose(window);
        else {
            UmiDocumentReplacementSetSummary summary;
            UmiStatus status=UmiDocumentReplacementSetInspect(review->set,&summary);
            int applied=0;
            if(status==UMI_STATUS_OK && !summary.applied) {
                if(GTK_WIDGET(button)==review->previous && review->selected>0U) --review->selected;
                else if(GTK_WIDGET(button)==review->next && review->selected+1U<summary.document_count) ++review->selected;
                else if(GTK_WIDGET(button)==review->mark) {
                    status=review->presented==review->selected?
                        UmiDocumentReplacementSetReview(review->set,review->selected,review->presented_revision):UMI_STATUS_INVALID_STATE;
                } else if(GTK_WIDGET(button)==review->apply) {
                    status=UmiDocumentReplacementSetApply(review->set,summary.revision,
                        gtk_check_button_get_active(GTK_CHECK_BUTTON(review->approval)));
                    applied=status==UMI_STATUS_OK;
                }
            }
            review->last_status=status;ReplacementSetRender(window,review);
            /* Refresh may detach the host. Reacquire after rendering and do
             * not access that owner again after notifying completion. */
            owner=ReplacementSetOwner(window,review);
            if(applied && owner!=NULL) (void)EditFinished(owner,UMI_STATUS_OK);
        }
    }
    g_object_unref(window);
}
#endif
static void ReplacementSetRespond(GtkButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    ReplacementSetReview *review = g_object_get_data(G_OBJECT(window), "umicom-replacement-set");
    UmiGtk4Adapter *owner = ReplacementSetOwner(window, review);
    if (owner != NULL && !review->rendering)
    {
        if (GTK_WIDGET(button) == review->close)
            ReplacementSetClose(window);
        else
        {
            UmiDocumentReplacementSetSummary summary;
            UmiStatus status = UmiDocumentReplacementSetInspect(review->set, &summary);
            int applied = 0;
            if (status == UMI_STATUS_OK && !summary.applied)
            {
                if (GTK_WIDGET(button) == review->previous && review->selected > 0U)
                    --review->selected;
                else if (GTK_WIDGET(button) == review->next && review->selected + 1U < summary.document_count)
                    ++review->selected;
                else if (GTK_WIDGET(button) == review->mark)
                {
                    status = review->presented == review->selected
                                 ? UmiDocumentReplacementSetReview(review->set, review->selected,
                                                                   review->presented_revision)
                                 : UMI_STATUS_INVALID_STATE;
                }
                else if (GTK_WIDGET(button) == review->apply)
                {
                    status = review->last_status != UMI_STATUS_OK || review->presented == SIZE_MAX
                                 ? UMI_STATUS_INVALID_STATE
                                 : UmiDocumentReplacementSetApply(
                                       review->set, summary.revision,
                                       gtk_check_button_get_active(GTK_CHECK_BUTTON(review->approval)));
                    applied = status == UMI_STATUS_OK;
                }
            }
            review->last_status = status;
            ReplacementSetRender(window, review);
            /* Refresh may detach the host. Reacquire after rendering and do
             * not access that owner again after notifying completion. */
            owner = ReplacementSetOwner(window, review);
            if (applied && owner != NULL)
                (void)EditFinished(owner, UMI_STATUS_OK);
        }
    }
    g_object_unref(window);
}

/* Reserve review creation on the original lifetime token before GTK observers can request another replacement window.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReviewReplacementSet(UmiGtk4Adapter *adapter,const char *needle,const char *replacement)
{
    if(adapter==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(adapter->edit_coordinator==NULL || adapter->edit_lifetime==NULL || adapter->window==NULL || !UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if(UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel!=NULL ||
        g_object_get_data(adapter->edit_lifetime,"location-dialog")!=NULL) return UMI_STATUS_BUSY;
    UmiDocumentReplacementSet *set=NULL;
    UmiStatus status=UmiDocumentReplacementSetCreate(adapter->edit_coordinator,needle,replacement,&set);
    if(status!=UMI_STATUS_OK) return status;
    GObject *lifetime=g_object_ref(adapter->edit_lifetime);GtkWindow *parent=g_object_ref(adapter->window);
    GtkWidget *window=gtk_window_new();g_object_ref(window);
    ReplacementSetReview *review=g_new0(ReplacementSetReview,1);review->lifetime=g_object_ref(lifetime);review->set=set;review->presented=SIZE_MAX;
    g_object_set_data_full(G_OBJECT(window),"umicom-replacement-set",review,ReplacementSetFree);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8),*buttons=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);
    review->message=g_object_ref_sink(gtk_label_new("Reviewing complete draft set..."));
    review->content=g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL,0));
    review->previous=g_object_ref_sink(gtk_button_new_with_label("Previous"));
    review->next=g_object_ref_sink(gtk_button_new_with_label("Next"));
    review->mark=g_object_ref_sink(gtk_button_new_with_label("Mark this draft reviewed"));
    review->approval=g_object_ref_sink(gtk_check_button_new_with_label("I approve all reviewed draft changes"));
    review->apply=g_object_ref_sink(gtk_button_new_with_label("Apply complete set"));
    review->close=g_object_ref_sink(gtk_button_new_with_label("Close"));
    gtk_label_set_wrap(GTK_LABEL(review->message),TRUE);gtk_label_set_xalign(GTK_LABEL(review->message),0.0F);
    gtk_widget_set_vexpand(review->content,TRUE);gtk_widget_set_sensitive(review->apply,FALSE);gtk_widget_set_sensitive(review->approval,FALSE);
    gtk_box_append(GTK_BOX(box),review->message);gtk_box_append(GTK_BOX(box),review->content);gtk_box_append(GTK_BOX(box),review->approval);
    GtkWidget *actions[]={review->previous,review->next,review->mark,review->apply,review->close};
    const char *ids[]={"previous","next","reviewed","apply","close"};
    for(size_t i=0U;i<sizeof(actions)/sizeof(actions[0]);++i) {
        gtk_box_append(GTK_BOX(buttons),actions[i]);char *tag=g_strdup_printf("document.replacement-set.%s",ids[i]);
        (void)umi_gtk4_automation_tag_widget(actions[i],tag);g_free(tag);
        g_signal_connect_object(actions[i],"clicked",G_CALLBACK(ReplacementSetRespond),window,0);
    }
    (void)umi_gtk4_automation_tag_widget(window,"document.replacement-set.review");
    (void)umi_gtk4_automation_tag_widget(review->message,"document.replacement-set.summary");
    (void)umi_gtk4_automation_tag_widget(review->approval,"document.replacement-set.approval");
    gtk_box_append(GTK_BOX(box),buttons);gtk_window_set_title(GTK_WINDOW(window),"Review complete replacement set");
    gtk_window_set_default_size(GTK_WINDOW(window),1050,720);gtk_window_set_transient_for(GTK_WINDOW(window),parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window),TRUE);gtk_window_set_modal(GTK_WINDOW(window),TRUE);gtk_window_set_child(GTK_WINDOW(window),box);
    g_signal_connect_object(review->approval,"toggled",G_CALLBACK(ReplacementSetApprovalChanged),window,0);
    g_signal_connect(window,"close-request",G_CALLBACK(ReplacementSetClosing),NULL);
    if(g_object_get_data(lifetime,"adapter")!=adapter || !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK) {
        g_object_set_data(lifetime,"replacement-set-dialog",window);
        status=UmiGtk4WatchWindowClosed(GTK_WINDOW(window),G_OBJECT(window),ReplacementSetRemoved,NULL);
        if(status==UMI_STATUS_OK) status=UmiGtk4WatchWindowClosed(parent,G_OBJECT(window),ReplacementSetParentRemoved,window);
        if(status==UMI_STATUS_OK && review->closed) status=UMI_STATUS_CANCELLED;
    }
    if(status==UMI_STATUS_OK) {
        ReplacementSetRender(window,review);
        if(!review->closed) {gtk_window_set_default_widget(GTK_WINDOW(window),review->close);gtk_window_set_focus(GTK_WINDOW(window),review->close);gtk_window_present(GTK_WINDOW(window));}
        else status=UMI_STATUS_CANCELLED;
    } else ReplacementSetClose(window);
    g_object_unref(parent);g_object_unref(lifetime);g_object_unref(window);return status;
}
#endif
/* Native replacement review now captures and displays explicit case and word policy; its original lifetime and approval flow remain shared.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReviewReplacementSet(UmiGtk4Adapter *adapter, const char *needle,
                                             const char *replacement)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentReplacementSet *set = NULL;
    UmiStatus status = UmiDocumentReplacementSetCreate(adapter->edit_coordinator, needle, replacement, &set);
    if (status != UMI_STATUS_OK)
        return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    g_object_set_data(lifetime, "replacement-set-creating", GINT_TO_POINTER(1));
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    ReplacementSetReview *review = g_new0(ReplacementSetReview, 1);
    review->lifetime = g_object_ref(lifetime);
    review->set = set;
    review->presented = SIZE_MAX;
    g_object_set_data_full(G_OBJECT(window), "umicom-replacement-set", review, ReplacementSetFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8),
              *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    review->message = g_object_ref_sink(gtk_label_new("Reviewing complete draft set..."));
    review->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    review->previous = g_object_ref_sink(gtk_button_new_with_label("Previous"));
    review->next = g_object_ref_sink(gtk_button_new_with_label("Next"));
    review->mark = g_object_ref_sink(gtk_button_new_with_label("Mark this draft reviewed"));
    review->approval =
        g_object_ref_sink(gtk_check_button_new_with_label("I approve all reviewed draft changes"));
    review->apply = g_object_ref_sink(gtk_button_new_with_label("Apply complete set"));
    review->close = g_object_ref_sink(gtk_button_new_with_label("Close"));
    gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(review->message), 0.0F);
    gtk_widget_set_vexpand(review->content, TRUE);
    gtk_widget_set_sensitive(review->apply, FALSE);
    gtk_widget_set_sensitive(review->approval, FALSE);
    gtk_box_append(GTK_BOX(box), review->message);
    gtk_box_append(GTK_BOX(box), review->content);
    gtk_box_append(GTK_BOX(box), review->approval);
    GtkWidget *actions[] = {review->previous, review->next, review->mark, review->apply, review->close};
    const char *ids[] = {"previous", "next", "reviewed", "apply", "close"};
    for (size_t i = 0U; i < sizeof(actions) / sizeof(actions[0]); ++i)
    {
        gtk_box_append(GTK_BOX(buttons), actions[i]);
        char *tag = g_strdup_printf("document.replacement-set.%s", ids[i]);
        (void)umi_gtk4_automation_tag_widget(actions[i], tag);
        g_free(tag);
        g_signal_connect_object(actions[i], "clicked", G_CALLBACK(ReplacementSetRespond), window, 0);
    }
    (void)umi_gtk4_automation_tag_widget(window, "document.replacement-set.review");
    (void)umi_gtk4_automation_tag_widget(review->message, "document.replacement-set.summary");
    (void)umi_gtk4_automation_tag_widget(review->approval, "document.replacement-set.approval");
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_title(GTK_WINDOW(window), "Review complete replacement set");
    gtk_window_set_default_size(GTK_WINDOW(window), 1050, 720);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_object(review->approval, "toggled", G_CALLBACK(ReplacementSetApprovalChanged), window,
                            0);
    g_signal_connect(window, "close-request", G_CALLBACK(ReplacementSetClosing), NULL);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        g_object_set_data(lifetime, "replacement-set-dialog", window);
        status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), ReplacementSetRemoved, NULL);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), ReplacementSetParentRemoved, window);
        if (status == UMI_STATUS_OK && review->closed)
            status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK)
    {
        ReplacementSetRender(window, review);
        if (!review->closed)
        {
            gtk_window_set_default_widget(GTK_WINDOW(window), review->close);
            gtk_window_set_focus(GTK_WINDOW(window), review->close);
            gtk_window_present(GTK_WINDOW(window));
        }
        else
            status = UMI_STATUS_CANCELLED;
    }
    else
        ReplacementSetClose(window);
    g_object_set_data(lifetime, "replacement-set-creating", NULL);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
#endif
UmiStatus UmiGtk4AdapterReviewReplacementSetWithOptions(UmiGtk4Adapter *adapter, const char *needle,
                                             const char *replacement, const UmiEditorSearchOptions *searchOptions)
{
    /* Copy before GTK publishes a new toplevel and notifies native observers. */
    UmiEditorSearchOptions capturedOptions = searchOptions != NULL ? *searchOptions :
        (UmiEditorSearchOptions){UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0U};

    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentReplacementSet *set = NULL;
    UmiStatus status = UmiDocumentReplacementSetCreateWithOptions(adapter->edit_coordinator, needle, replacement, &capturedOptions, &set);
    if (status != UMI_STATUS_OK)
        return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    g_object_set_data(lifetime, "replacement-set-creating", GINT_TO_POINTER(1));
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    ReplacementSetReview *review = g_new0(ReplacementSetReview, 1);
    review->lifetime = g_object_ref(lifetime);
    review->set = set;
    review->presented = SIZE_MAX;
    g_object_set_data_full(G_OBJECT(window), "umicom-replacement-set", review, ReplacementSetFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8),
              *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    review->message = g_object_ref_sink(gtk_label_new("Reviewing complete draft set..."));
    review->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    review->previous = g_object_ref_sink(gtk_button_new_with_label("Previous"));
    review->next = g_object_ref_sink(gtk_button_new_with_label("Next"));
    review->mark = g_object_ref_sink(gtk_button_new_with_label("Mark this draft reviewed"));
    review->approval =
        g_object_ref_sink(gtk_check_button_new_with_label("I approve all reviewed draft changes"));
    review->apply = g_object_ref_sink(gtk_button_new_with_label("Apply complete set"));
    review->close = g_object_ref_sink(gtk_button_new_with_label("Close"));
    gtk_label_set_wrap(GTK_LABEL(review->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(review->message), 0.0F);
    gtk_widget_set_vexpand(review->content, TRUE);
    gtk_widget_set_sensitive(review->apply, FALSE);
    gtk_widget_set_sensitive(review->approval, FALSE);
    GtkWidget *policyLabel = gtk_label_new(ReplacementSearchPolicyLabel(&capturedOptions));
    gtk_label_set_xalign(GTK_LABEL(policyLabel), 0.0F);
    (void)umi_gtk4_automation_tag_widget(policyLabel, "document.replacements.search-policy");
    gtk_box_append(GTK_BOX(box), policyLabel);
    gtk_box_append(GTK_BOX(box), review->message);
    gtk_box_append(GTK_BOX(box), review->content);
    gtk_box_append(GTK_BOX(box), review->approval);
    GtkWidget *actions[] = {review->previous, review->next, review->mark, review->apply, review->close};
    const char *ids[] = {"previous", "next", "reviewed", "apply", "close"};
    for (size_t i = 0U; i < sizeof(actions) / sizeof(actions[0]); ++i)
    {
        gtk_box_append(GTK_BOX(buttons), actions[i]);
        char *tag = g_strdup_printf("document.replacement-set.%s", ids[i]);
        (void)umi_gtk4_automation_tag_widget(actions[i], tag);
        g_free(tag);
        g_signal_connect_object(actions[i], "clicked", G_CALLBACK(ReplacementSetRespond), window, 0);
    }
    (void)umi_gtk4_automation_tag_widget(window, "document.replacement-set.review");
    (void)umi_gtk4_automation_tag_widget(review->message, "document.replacement-set.summary");
    (void)umi_gtk4_automation_tag_widget(review->approval, "document.replacement-set.approval");
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_title(GTK_WINDOW(window), "Review complete replacement set");
    gtk_window_set_default_size(GTK_WINDOW(window), 1050, 720);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_object(review->approval, "toggled", G_CALLBACK(ReplacementSetApprovalChanged), window,
                            0);
    g_signal_connect(window, "close-request", G_CALLBACK(ReplacementSetClosing), NULL);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        g_object_set_data(lifetime, "replacement-set-dialog", window);
        status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), ReplacementSetRemoved, NULL);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), ReplacementSetParentRemoved, window);
        if (status == UMI_STATUS_OK && review->closed)
            status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK)
    {
        ReplacementSetRender(window, review);
        if (!review->closed)
        {
            gtk_window_set_default_widget(GTK_WINDOW(window), review->close);
            gtk_window_set_focus(GTK_WINDOW(window), review->close);
            gtk_window_present(GTK_WINDOW(window));
        }
        else
            status = UMI_STATUS_CANCELLED;
    }
    else
        ReplacementSetClose(window);
    g_object_set_data(lifetime, "replacement-set-creating", NULL);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
UmiStatus UmiGtk4AdapterReviewReplacementSet(UmiGtk4Adapter *adapter, const char *needle, const char *replacement)
{
    return UmiGtk4AdapterReviewReplacementSetWithOptions(adapter, needle, replacement, NULL);
}

