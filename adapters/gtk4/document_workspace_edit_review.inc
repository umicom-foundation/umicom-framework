/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_workspace_edit_review.inc
 * PURPOSE: Keep complete language edits and their confirmations under the native document owner lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workspace_edit_review.h"
#include "umicom/ui/gtk4/interaction_recording.h"
typedef struct WorkspaceEditPanel
{
    GObject *lifetime;
    UmiSourceWorkspaceEditReview *model;
    GtkWidget *message, *content, *details, *previous, *next, *mark, *approval, *apply, *close;
    GPtrArray *annotations;
    size_t selected, presented;
    uint64_t presented_revision;
    UmiStatus last_status;
    int closed, rendering;
} WorkspaceEditPanel;
static WorkspaceEditPanel *WorkspacePanelState(GtkWidget *window)
{
    return g_object_get_data(G_OBJECT(window), "umicom-workspace-edit-review");
}
static UmiGtk4Adapter *WorkspacePanelOwner(GtkWidget *window, WorkspaceEditPanel *panel)
{
    if (panel == NULL || panel->closed ||
        g_object_get_data(panel->lifetime, "workspace-edit-dialog") != window)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(panel->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void WorkspacePanelFree(gpointer data)
{
    WorkspaceEditPanel *panel = data;
    UmiSourceWorkspaceEditReviewDestroy(panel->model);
    g_clear_object(&panel->message);
    g_clear_object(&panel->content);
    g_clear_object(&panel->details);
    g_clear_object(&panel->previous);
    g_clear_object(&panel->next);
    g_clear_object(&panel->mark);
    g_clear_object(&panel->approval);
    g_clear_object(&panel->apply);
    g_clear_object(&panel->close);
    g_ptr_array_unref(panel->annotations);
    g_clear_object(&panel->lifetime);
    g_free(panel);
}
static void WorkspacePanelRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    WorkspaceEditPanel *panel = WorkspacePanelState(window);
    if (panel == NULL || panel->closed)
        return;
    panel->closed = 1;
    panel->presented = SIZE_MAX;
    panel->presented_revision = 0U;
    if (g_object_get_data(panel->lifetime, "workspace-edit-dialog") == window)
        g_object_set_data(panel->lifetime, "workspace-edit-dialog", NULL);
}
static void WorkspacePanelClose(GtkWidget *window)
{
    g_object_ref(window);
    WorkspacePanelRemoved(window, NULL);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
}
static void WorkspacePanelParentRemoved(GtkWidget *parent, gpointer data)
{
    (void)parent;
    WorkspacePanelClose(GTK_WIDGET(data));
}
static void WorkspacePanelDetach(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "workspace-edit-dialog");
    if (window != NULL)
        WorkspacePanelClose(window);
}
static gboolean WorkspacePanelClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    WorkspacePanelClose(GTK_WIDGET(window));
    return TRUE;
}
/* Provider text is never interpreted as markup. The complete comparison owns
 * its displayed copies; the model retains exact source and annotation bytes. */
/* Display the shared 64-bit UTF-16 position fields with matching format types; retain the initial range display for review.
 * The former implementation is retained for engineering review. */
#if 0
static void WorkspacePanelRender(GtkWidget *window,WorkspaceEditPanel *panel)
{
    if(WorkspacePanelOwner(window,panel)==NULL || panel->rendering) return;
    panel->rendering=1;panel->presented=SIZE_MAX;panel->presented_revision=0U;
    UmiSourceWorkspaceEditReviewSummary summary={0};UmiDocumentSourceRequestSummary document={0};int reviewed=0;
    UmiStatus status=UmiSourceWorkspaceEditReviewInspect(panel->model,&summary);
    if(status==UMI_STATUS_OK) status=UmiSourceWorkspaceEditReviewAt(panel->model,panel->selected,&document,&reviewed);
    GtkWidget *child;while((child=gtk_widget_get_first_child(panel->content))!=NULL) gtk_box_remove(GTK_BOX(panel->content),child);
    if(status==UMI_STATUS_OK && !summary.applied && WorkspacePanelOwner(window,panel)!=NULL) {
        const char *source=NULL,*proposed=NULL;size_t source_bytes=0U,proposed_bytes=0U;GtkWidget *comparison=NULL;
        status=UmiSourceWorkspaceEditReviewTexts(panel->model,panel->selected,&source,&source_bytes,&proposed,&proposed_bytes);
        if(status==UMI_STATUS_OK) status=UmiGtk4TextComparisonCreate(source,source_bytes,proposed,proposed_bytes,"Captured draft","Proposed draft",&comparison);
        if(comparison!=NULL) {
            g_object_ref_sink(comparison);
            if(WorkspacePanelOwner(window,panel)!=NULL) gtk_box_append(GTK_BOX(panel->content),comparison);
            g_object_unref(comparison);
        }
        if(status==UMI_STATUS_OK && comparison!=NULL && WorkspacePanelOwner(window,panel)!=NULL) {
            panel->presented=panel->selected;panel->presented_revision=summary.revision;
        }
    }
    const UmiLanguageWorkspaceEditCatalogue *catalogue=NULL;
    if(status==UMI_STATUS_OK) status=UmiSourceWorkspaceEditReviewCatalogue(panel->model,&catalogue);
    GString *details=g_string_new(NULL);
    UmiLanguageWorkspaceDocumentChange change;
    if(status==UMI_STATUS_OK) status=UmiLanguageWorkspaceEditCatalogueDocument(catalogue,panel->selected,&change);
    if(status==UMI_STATUS_OK) g_string_append_printf(details,"Target: %s\nRanges use zero-based lines and UTF-16 characters.\n",change.uri);
    for(size_t i=0U;status==UMI_STATUS_OK && i<change.edit_count;++i) {
        UmiLanguageWorkspaceTextChange edit;status=UmiLanguageWorkspaceEditCatalogueEdit(catalogue,panel->selected,i,&edit);
        if(status!=UMI_STATUS_OK) break;
        g_string_append_printf(details,"Edit %zu: %u:%u to %u:%u",i+1U,edit.range.start.line,edit.range.start.character,edit.range.end.line,edit.range.end.character);
        if(edit.annotation!=SIZE_MAX) g_string_append_printf(details,"; explanation %zu",edit.annotation+1U);
        g_string_append_c(details,'\n');
    }
    if(WorkspacePanelOwner(window,panel)!=NULL) gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->details)),details->str,-1);
    g_string_free(details,TRUE);
    if(status!=UMI_STATUS_OK) panel->last_status=status;
    if(WorkspacePanelOwner(window,panel)!=NULL) {
        char *message=summary.applied?
            g_strdup_printf("Applied %zu changed drafts. No files were saved. Undo is available separately in each document.",summary.changed_count):
            g_strdup_printf("Document %zu of %zu: %s\n%zu of %zu changed drafts reviewed. %zu of %zu required explanations confirmed.\n%zu captured source dependencies are checked before application.%s%s",panel->selected+1U,summary.document_count,document.display_name,summary.reviewed_count,summary.changed_count,summary.confirmed_annotations,summary.required_annotations,summary.dependency_count,panel->last_status==UMI_STATUS_OK?"":"\nLast action: ",panel->last_status==UMI_STATUS_OK?"":umi_status_text(panel->last_status));
        gtk_label_set_text(GTK_LABEL(panel->message),message);g_free(message);
        int ready=!summary.applied && panel->last_status==UMI_STATUS_OK && WorkspacePanelOwner(window,panel)!=NULL;
        gtk_widget_set_sensitive(panel->previous,ready && panel->selected>0U);
        gtk_widget_set_sensitive(panel->next,ready && panel->selected+1U<summary.document_count);
        gtk_widget_set_sensitive(panel->mark,ready && panel->presented==panel->selected && document.text_changes && !reviewed);
        int permitted=ready && summary.reviewed_count==summary.changed_count && summary.confirmed_annotations==summary.required_annotations;
        gtk_widget_set_sensitive(panel->approval,permitted);
        gtk_widget_set_sensitive(panel->apply,permitted && gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)));
        for(guint i=0U;i<panel->annotations->len;++i) {
            GtkWidget *control=g_ptr_array_index(panel->annotations,i);int required=0,confirmed=0;
            if(UmiSourceWorkspaceEditReviewAnnotationState(panel->model,(size_t)i,&required,&confirmed)==UMI_STATUS_OK) {
                gtk_check_button_set_active(GTK_CHECK_BUTTON(control),confirmed);
                gtk_widget_set_sensitive(control,ready && required);
            }
        }
    }
    panel->rendering=0;
}
#endif
static void WorkspacePanelRender(GtkWidget *window, WorkspaceEditPanel *panel)
{
    if (WorkspacePanelOwner(window, panel) == NULL || panel->rendering)
        return;
    panel->rendering = 1;
    panel->presented = SIZE_MAX;
    panel->presented_revision = 0U;
    UmiSourceWorkspaceEditReviewSummary summary = {0};
    UmiDocumentSourceRequestSummary document = {0};
    int reviewed = 0;
    UmiStatus status = UmiSourceWorkspaceEditReviewInspect(panel->model, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiSourceWorkspaceEditReviewAt(panel->model, panel->selected, &document, &reviewed);
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->content)) != NULL)
        gtk_box_remove(GTK_BOX(panel->content), child);
    if (status == UMI_STATUS_OK && !summary.applied && WorkspacePanelOwner(window, panel) != NULL)
    {
        const char *source = NULL, *proposed = NULL;
        size_t source_bytes = 0U, proposed_bytes = 0U;
        GtkWidget *comparison = NULL;
        status = UmiSourceWorkspaceEditReviewTexts(panel->model, panel->selected, &source, &source_bytes,
                                                   &proposed, &proposed_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4TextComparisonCreate(source, source_bytes, proposed, proposed_bytes,
                                                 "Captured draft", "Proposed draft", &comparison);
        if (comparison != NULL)
        {
            g_object_ref_sink(comparison);
            if (WorkspacePanelOwner(window, panel) != NULL)
                gtk_box_append(GTK_BOX(panel->content), comparison);
            g_object_unref(comparison);
        }
        if (status == UMI_STATUS_OK && comparison != NULL && WorkspacePanelOwner(window, panel) != NULL)
        {
            panel->presented = panel->selected;
            panel->presented_revision = summary.revision;
        }
    }
    const UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiSourceWorkspaceEditReviewCatalogue(panel->model, &catalogue);
    GString *details = g_string_new(NULL);
    UmiLanguageWorkspaceDocumentChange change;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageWorkspaceEditCatalogueDocument(catalogue, panel->selected, &change);
    if (status == UMI_STATUS_OK)
        g_string_append_printf(details, "Target: %s\nRanges use zero-based lines and UTF-16 characters.\n",
                               change.uri);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < change.edit_count; ++i)
    {
        UmiLanguageWorkspaceTextChange edit;
        status = UmiLanguageWorkspaceEditCatalogueEdit(catalogue, panel->selected, i, &edit);
        if (status != UMI_STATUS_OK)
            break;
        g_string_append_printf(
            details, "Edit %zu: %llu:%llu to %llu:%llu", i + 1U, (unsigned long long)edit.range.start.line,
            (unsigned long long)edit.range.start.utf16_column, (unsigned long long)edit.range.end.line,
            (unsigned long long)edit.range.end.utf16_column);
        if (edit.annotation != SIZE_MAX)
            g_string_append_printf(details, "; explanation %zu", edit.annotation + 1U);
        g_string_append_c(details, '\n');
    }
    if (WorkspacePanelOwner(window, panel) != NULL)
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->details)), details->str, -1);
    g_string_free(details, TRUE);
    if (status != UMI_STATUS_OK)
        panel->last_status = status;
    if (WorkspacePanelOwner(window, panel) != NULL)
    {
        char *message =
            summary.applied
                ? g_strdup_printf("Applied %zu changed drafts. No files were saved. Undo is available "
                                  "separately in each document.",
                                  summary.changed_count)
                : g_strdup_printf(
                      "Document %zu of %zu: %s\n%zu of %zu changed drafts reviewed. %zu of %zu required "
                      "explanations confirmed.\n%zu captured source dependencies are checked before "
                      "application.%s%s",
                      panel->selected + 1U, summary.document_count, document.display_name,
                      summary.reviewed_count, summary.changed_count, summary.confirmed_annotations,
                      summary.required_annotations, summary.dependency_count,
                      panel->last_status == UMI_STATUS_OK ? "" : "\nLast action: ",
                      panel->last_status == UMI_STATUS_OK ? "" : umi_status_text(panel->last_status));
        gtk_label_set_text(GTK_LABEL(panel->message), message);
        g_free(message);
        int ready = !summary.applied && panel->last_status == UMI_STATUS_OK &&
                    WorkspacePanelOwner(window, panel) != NULL;
        gtk_widget_set_sensitive(panel->previous, ready && panel->selected > 0U);
        gtk_widget_set_sensitive(panel->next, ready && panel->selected + 1U < summary.document_count);
        gtk_widget_set_sensitive(panel->mark, ready && panel->presented == panel->selected &&
                                                  document.text_changes && !reviewed);
/* Zero reviewed changes previously satisfied the equality gate and enabled approval. Require an actual text change while retaining the earlier gate for review. */
#if 0
        int permitted = ready && summary.reviewed_count == summary.changed_count &&
                        summary.confirmed_annotations == summary.required_annotations;
#endif
        /* An empty proposal has nothing to approve. Keep inspection and Close
         * available without presenting a successful edit that changes no draft. */
        int permitted = ready && summary.changed_count != 0U &&
                        summary.reviewed_count == summary.changed_count &&
                        summary.confirmed_annotations == summary.required_annotations;
        gtk_widget_set_sensitive(panel->approval, permitted);
        gtk_widget_set_sensitive(panel->apply,
                                 permitted && gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)));
        for (guint i = 0U; i < panel->annotations->len; ++i)
        {
            GtkWidget *control = g_ptr_array_index(panel->annotations, i);
            int required = 0, confirmed = 0;
            if (UmiSourceWorkspaceEditReviewAnnotationState(panel->model, (size_t)i, &required, &confirmed) ==
                UMI_STATUS_OK)
            {
                gtk_check_button_set_active(GTK_CHECK_BUTTON(control), confirmed);
                gtk_widget_set_sensitive(control, ready && required);
            }
        }
    }
    panel->rendering = 0;
}
static void WorkspacePanelApprovalChanged(GtkCheckButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    WorkspaceEditPanel *panel = WorkspacePanelState(window);
    if (WorkspacePanelOwner(window, panel) != NULL && !panel->rendering)
    {
        if (GTK_WIDGET(button) != panel->approval)
        {
            size_t index = (size_t)GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "annotation-index"));
            UmiSourceWorkspaceEditReviewSummary summary;
            UmiStatus status = UmiSourceWorkspaceEditReviewInspect(panel->model, &summary);
            if (status == UMI_STATUS_OK)
                status = UmiSourceWorkspaceEditReviewConfirmAnnotation(
                    panel->model, index, summary.revision, gtk_check_button_get_active(button) ? 1 : 0);
            panel->last_status = status;
            /* Any changed annotation decision retires the final approval. */
            panel->rendering = 1;
            gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approval), FALSE);
            panel->rendering = 0;
        }
        WorkspacePanelRender(window, panel);
    }
    g_object_unref(window);
}
static void WorkspacePanelRespond(GtkButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    WorkspaceEditPanel *panel = WorkspacePanelState(window);
    UmiGtk4Adapter *owner = WorkspacePanelOwner(window, panel);
    if (owner != NULL && !panel->rendering)
    {
        if (GTK_WIDGET(button) == panel->close)
            WorkspacePanelClose(window);
        else
        {
            UmiSourceWorkspaceEditReviewSummary summary;
            int applied = 0;
            UmiStatus status = UmiSourceWorkspaceEditReviewInspect(panel->model, &summary);
            if (status == UMI_STATUS_OK && !summary.applied)
            {
                if (GTK_WIDGET(button) == panel->previous && panel->selected > 0U)
                    --panel->selected;
                else if (GTK_WIDGET(button) == panel->next && panel->selected + 1U < summary.document_count)
                    ++panel->selected;
                else if (GTK_WIDGET(button) == panel->mark)
                    status = panel->presented == panel->selected
                                 ? UmiSourceWorkspaceEditReviewAcceptDocument(panel->model, panel->selected,
                                                                              panel->presented_revision)
                                 : UMI_STATUS_INVALID_STATE;
                else if (GTK_WIDGET(button) == panel->apply)
                {
/* The callback enforces the same nonempty proposal rule as the controls. Retain the prior activation guard for review; model-level callers keep their existing API. */
#if 0
                    status = panel->last_status == UMI_STATUS_OK && panel->presented != SIZE_MAX
                                 ? UmiSourceWorkspaceEditReviewApply(
                                       panel->model, summary.revision,
                                       gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
                                 : UMI_STATUS_INVALID_STATE;
#endif
                    /* Recheck the model at activation: retaining or emitting
                     * a disabled button must not publish an empty operation. */
                    status = panel->last_status == UMI_STATUS_OK &&
                             panel->presented != SIZE_MAX && summary.changed_count != 0U
                                 ? UmiSourceWorkspaceEditReviewApply(
                                       panel->model, summary.revision,
                                       gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approval)))
                                 : UMI_STATUS_INVALID_STATE;
                    applied = status == UMI_STATUS_OK;
                }
            }
            panel->last_status = status;
            WorkspacePanelRender(window, panel);
            owner = WorkspacePanelOwner(window, panel);
            if (applied && owner != NULL)
                (void)EditFinished(owner, UMI_STATUS_OK);
        }
    }
    g_object_unref(window);
}
/* Keep source and provider explanations private in interaction recordings, consistent with the source request form.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiGtk4AdapterReviewWorkspaceEdits(UmiGtk4Adapter *adapter,UmiSourceWorkspaceEditReview **review)
{
    if(adapter==NULL || review==NULL || *review==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(adapter->edit_coordinator==NULL || adapter->edit_lifetime==NULL || adapter->window==NULL || !UmiGtk4WindowIsOpen(adapter->window)) return UMI_STATUS_UNAVAILABLE;
    if(UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel!=NULL || g_object_get_data(adapter->edit_lifetime,"location-dialog")!=NULL) return UMI_STATUS_BUSY;
    UmiStatus status=UmiSourceWorkspaceEditReviewCheckOwner(*review,adapter->edit_coordinator);
    if(status!=UMI_STATUS_OK) return status;
    GObject *lifetime=g_object_ref(adapter->edit_lifetime);GtkWindow *parent=g_object_ref(adapter->window);
    /* Reserve ownership before top-level creation can synchronously invoke a
     * host observer. No other review can enter the same document transaction. */
    g_object_set_data(lifetime,"workspace-edit-creating",GINT_TO_POINTER(1));
    UmiSourceWorkspaceEditReview *model=*review;*review=NULL;
    GtkWidget *window=gtk_window_new();g_object_ref(window);
    WorkspaceEditPanel *panel=g_new0(WorkspaceEditPanel,1);panel->lifetime=g_object_ref(lifetime);panel->model=model;panel->presented=SIZE_MAX;
    panel->annotations=g_ptr_array_new_with_free_func(g_object_unref);
    g_object_set_data_full(G_OBJECT(window),"umicom-workspace-edit-review",panel,WorkspacePanelFree);
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL,8),*buttons=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);
    panel->message=g_object_ref_sink(gtk_label_new("Preparing complete workspace edit..."));
    panel->content=g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL,0));
    panel->details=g_object_ref_sink(gtk_text_view_new());gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->details),FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(panel->details),FALSE);gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->details),GTK_WRAP_WORD_CHAR);
    GtkWidget *explanations=gtk_box_new(GTK_ORIENTATION_VERTICAL,6),*scroll=gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(explanations),panel->details);
    const UmiLanguageWorkspaceEditCatalogue *catalogue=NULL;
    status=UmiSourceWorkspaceEditReviewCatalogue(panel->model,&catalogue);
    for(size_t i=0U;status==UMI_STATUS_OK && i<UmiLanguageWorkspaceEditCatalogueAnnotationCount(catalogue);++i) {
        UmiLanguageWorkspaceChangeAnnotation annotation;int required=0,confirmed=0;
        status=UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue,i,&annotation);
        if(status==UMI_STATUS_OK) status=UmiSourceWorkspaceEditReviewAnnotationState(panel->model,i,&required,&confirmed);
        if(status!=UMI_STATUS_OK) break;
        char *text=g_strdup_printf("Explanation %zu: %s\n%s\n%s",i+1U,annotation.label,annotation.description==NULL?"":annotation.description,required?"Confirm this explanation after reviewing its associated edits.":"No confirmation is required for this explanation.");
        GtkWidget *control=g_object_ref_sink(gtk_check_button_new()),*label=gtk_label_new(text);g_free(text);
        gtk_label_set_wrap(GTK_LABEL(label),TRUE);gtk_label_set_xalign(GTK_LABEL(label),0.0F);gtk_check_button_set_child(GTK_CHECK_BUTTON(control),label);
        gtk_widget_set_sensitive(control,required);g_ptr_array_add(panel->annotations,control);gtk_box_append(GTK_BOX(explanations),control);
        g_object_set_data(G_OBJECT(control),"annotation-index",GUINT_TO_POINTER((guint)i));
        char *tag=g_strdup_printf("document.workspace-edit.annotation.%zu",i);(void)umi_gtk4_automation_tag_widget(control,tag);g_free(tag);
        g_signal_connect_object(control,"toggled",G_CALLBACK(WorkspacePanelApprovalChanged),window,0);
    }
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),explanations);gtk_widget_set_size_request(scroll,-1,160);
    panel->previous=g_object_ref_sink(gtk_button_new_with_label("Previous"));panel->next=g_object_ref_sink(gtk_button_new_with_label("Next"));
    panel->mark=g_object_ref_sink(gtk_button_new_with_label("Mark this draft reviewed"));
    panel->approval=g_object_ref_sink(gtk_check_button_new_with_label("I approve the complete reviewed workspace edit"));
    panel->apply=g_object_ref_sink(gtk_button_new_with_label("Apply complete edit"));panel->close=g_object_ref_sink(gtk_button_new_with_label("Close"));
    gtk_label_set_wrap(GTK_LABEL(panel->message),TRUE);gtk_label_set_xalign(GTK_LABEL(panel->message),0.0F);
    gtk_widget_set_vexpand(panel->content,TRUE);gtk_widget_set_sensitive(panel->approval,FALSE);gtk_widget_set_sensitive(panel->apply,FALSE);
    gtk_box_append(GTK_BOX(box),panel->message);gtk_box_append(GTK_BOX(box),panel->content);gtk_box_append(GTK_BOX(box),scroll);gtk_box_append(GTK_BOX(box),panel->approval);
    GtkWidget *actions[]={panel->previous,panel->next,panel->mark,panel->apply,panel->close};const char *ids[]={"previous","next","reviewed","apply","close"};
    for(size_t i=0U;i<sizeof(actions)/sizeof(actions[0]);++i) {
        gtk_box_append(GTK_BOX(buttons),actions[i]);char *tag=g_strdup_printf("document.workspace-edit.%s",ids[i]);
        (void)umi_gtk4_automation_tag_widget(actions[i],tag);g_free(tag);
        g_signal_connect_object(actions[i],"clicked",G_CALLBACK(WorkspacePanelRespond),window,0);
    }
    (void)umi_gtk4_automation_tag_widget(window,"document.workspace-edit.review");
    (void)umi_gtk4_automation_tag_widget(panel->message,"document.workspace-edit.summary");
    (void)umi_gtk4_automation_tag_widget(panel->details,"document.workspace-edit.details");
    (void)umi_gtk4_automation_tag_widget(panel->approval,"document.workspace-edit.approval");
    gtk_box_append(GTK_BOX(box),buttons);gtk_window_set_title(GTK_WINDOW(window),"Review complete workspace edit");
    gtk_window_set_default_size(GTK_WINDOW(window),1100,800);gtk_window_set_transient_for(GTK_WINDOW(window),parent);gtk_window_set_destroy_with_parent(GTK_WINDOW(window),TRUE);gtk_window_set_modal(GTK_WINDOW(window),TRUE);gtk_window_set_child(GTK_WINDOW(window),box);
    g_signal_connect_object(panel->approval,"toggled",G_CALLBACK(WorkspacePanelApprovalChanged),window,0);
    g_signal_connect(window,"close-request",G_CALLBACK(WorkspacePanelClosing),NULL);
    if(g_object_get_data(lifetime,"adapter")!=adapter || !UmiGtk4WindowIsOpen(parent) || !UmiGtk4WindowIsOpen(GTK_WINDOW(window))) status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK) {
        g_object_set_data(lifetime,"workspace-edit-dialog",window);
        status=UmiGtk4WatchWindowClosed(GTK_WINDOW(window),G_OBJECT(window),WorkspacePanelRemoved,NULL);
        if(status==UMI_STATUS_OK) status=UmiGtk4WatchWindowClosed(parent,G_OBJECT(window),WorkspacePanelParentRemoved,window);
        if(status==UMI_STATUS_OK && panel->closed) status=UMI_STATUS_CANCELLED;
    }
    if(status==UMI_STATUS_OK) {
        WorkspacePanelRender(window,panel);
        if(!panel->closed) {gtk_window_set_default_widget(GTK_WINDOW(window),panel->close);gtk_window_set_focus(GTK_WINDOW(window),panel->close);gtk_window_present(GTK_WINDOW(window));}
        else status=UMI_STATUS_CANCELLED;
    } else WorkspacePanelClose(window);
    g_object_set_data(lifetime,"workspace-edit-creating",NULL);g_object_unref(parent);g_object_unref(lifetime);g_object_unref(window);return status;
}
#endif
UmiStatus UmiGtk4AdapterReviewWorkspaceEdits(UmiGtk4Adapter *adapter, UmiSourceWorkspaceEditReview **review)
{
    if (adapter == NULL || review == NULL || *review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterDocumentSaveAllBusy(adapter) || adapter->edit_cancel != NULL ||
        g_object_get_data(adapter->edit_lifetime, "location-dialog") != NULL)
        return UMI_STATUS_BUSY;
    UmiStatus status = UmiSourceWorkspaceEditReviewCheckOwner(*review, adapter->edit_coordinator);
    if (status != UMI_STATUS_OK)
        return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    /* Reserve ownership before top-level creation can synchronously invoke a
     * host observer. No other review can enter the same document transaction. */
    g_object_set_data(lifetime, "workspace-edit-creating", GINT_TO_POINTER(1));
    UmiSourceWorkspaceEditReview *model = *review;
    *review = NULL;
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    WorkspaceEditPanel *panel = g_new0(WorkspaceEditPanel, 1);
    panel->lifetime = g_object_ref(lifetime);
    panel->model = model;
    panel->presented = SIZE_MAX;
    panel->annotations = g_ptr_array_new_with_free_func(g_object_unref);
    g_object_set_data_full(G_OBJECT(window), "umicom-workspace-edit-review", panel, WorkspacePanelFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8),
              *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    panel->message = g_object_ref_sink(gtk_label_new("Preparing complete workspace edit..."));
    panel->content = g_object_ref_sink(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    panel->details = g_object_ref_sink(gtk_text_view_new());
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->details), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(panel->details), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->details), GTK_WRAP_WORD_CHAR);
    GtkWidget *explanations = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6), *scroll = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(explanations), panel->details);
    const UmiLanguageWorkspaceEditCatalogue *catalogue = NULL;
    status = UmiSourceWorkspaceEditReviewCatalogue(panel->model, &catalogue);
    for (size_t i = 0U;
         status == UMI_STATUS_OK && i < UmiLanguageWorkspaceEditCatalogueAnnotationCount(catalogue); ++i)
    {
        UmiLanguageWorkspaceChangeAnnotation annotation;
        int required = 0, confirmed = 0;
        status = UmiLanguageWorkspaceEditCatalogueAnnotation(catalogue, i, &annotation);
        if (status == UMI_STATUS_OK)
            status = UmiSourceWorkspaceEditReviewAnnotationState(panel->model, i, &required, &confirmed);
        if (status != UMI_STATUS_OK)
            break;
        char *text =
            g_strdup_printf("Explanation %zu: %s\n%s\n%s", i + 1U, annotation.label,
                            annotation.description == NULL ? "" : annotation.description,
                            required ? "Confirm this explanation after reviewing its associated edits."
                                     : "No confirmation is required for this explanation.");
        GtkWidget *control = g_object_ref_sink(gtk_check_button_new()), *label = gtk_label_new(text);
        g_free(text);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_check_button_set_child(GTK_CHECK_BUTTON(control), label);
        gtk_widget_set_sensitive(control, required);
        g_ptr_array_add(panel->annotations, control);
        gtk_box_append(GTK_BOX(explanations), control);
        g_object_set_data(G_OBJECT(control), "annotation-index", GUINT_TO_POINTER((guint)i));
        char *tag = g_strdup_printf("document.workspace-edit.annotation.%zu", i);
        (void)umi_gtk4_automation_tag_widget(control, tag);
        g_free(tag);
        g_signal_connect_object(control, "toggled", G_CALLBACK(WorkspacePanelApprovalChanged), window, 0);
    }
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), explanations);
    gtk_widget_set_size_request(scroll, -1, 160);
    panel->previous = g_object_ref_sink(gtk_button_new_with_label("Previous"));
    panel->next = g_object_ref_sink(gtk_button_new_with_label("Next"));
    panel->mark = g_object_ref_sink(gtk_button_new_with_label("Mark this draft reviewed"));
    panel->approval =
        g_object_ref_sink(gtk_check_button_new_with_label("I approve the complete reviewed workspace edit"));
    panel->apply = g_object_ref_sink(gtk_button_new_with_label("Apply complete edit"));
    panel->close = g_object_ref_sink(gtk_button_new_with_label("Close"));
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->message), 0.0F);
    gtk_widget_set_vexpand(panel->content, TRUE);
    gtk_widget_set_sensitive(panel->approval, FALSE);
    gtk_widget_set_sensitive(panel->apply, FALSE);
    gtk_box_append(GTK_BOX(box), panel->message);
    gtk_box_append(GTK_BOX(box), panel->content);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_box_append(GTK_BOX(box), panel->approval);
    GtkWidget *actions[] = {panel->previous, panel->next, panel->mark, panel->apply, panel->close};
    const char *ids[] = {"previous", "next", "reviewed", "apply", "close"};
    for (size_t i = 0U; i < sizeof(actions) / sizeof(actions[0]); ++i)
    {
        gtk_box_append(GTK_BOX(buttons), actions[i]);
        char *tag = g_strdup_printf("document.workspace-edit.%s", ids[i]);
        (void)umi_gtk4_automation_tag_widget(actions[i], tag);
        g_free(tag);
        g_signal_connect_object(actions[i], "clicked", G_CALLBACK(WorkspacePanelRespond), window, 0);
    }
    (void)umi_gtk4_automation_tag_widget(window, "document.workspace-edit.review");
    (void)UmiGtk4RecordingSetPrivate(window, 1);
    (void)umi_gtk4_automation_tag_widget(panel->message, "document.workspace-edit.summary");
    (void)umi_gtk4_automation_tag_widget(panel->details, "document.workspace-edit.details");
    (void)umi_gtk4_automation_tag_widget(panel->approval, "document.workspace-edit.approval");
    gtk_box_append(GTK_BOX(box), buttons);
    gtk_window_set_title(GTK_WINDOW(window), "Review complete workspace edit");
    gtk_window_set_default_size(GTK_WINDOW(window), 1100, 800);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);
    g_signal_connect_object(panel->approval, "toggled", G_CALLBACK(WorkspacePanelApprovalChanged), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(WorkspacePanelClosing), NULL);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
    {
        g_object_set_data(lifetime, "workspace-edit-dialog", window);
        status = UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), WorkspacePanelRemoved, NULL);
        if (status == UMI_STATUS_OK)
            status = UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), WorkspacePanelParentRemoved, window);
        if (status == UMI_STATUS_OK && panel->closed)
            status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK)
    {
        WorkspacePanelRender(window, panel);
        if (!panel->closed)
        {
            gtk_window_set_default_widget(GTK_WINDOW(window), panel->close);
            gtk_window_set_focus(GTK_WINDOW(window), panel->close);
            gtk_window_present(GTK_WINDOW(window));
        }
        else
            status = UMI_STATUS_CANCELLED;
    }
    else
        WorkspacePanelClose(window);
    g_object_set_data(lifetime, "workspace-edit-creating", NULL);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
