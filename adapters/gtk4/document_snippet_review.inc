/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_snippet_review.inc
 * PURPOSE: Keep local snippet composition and explicit document approval in the shared GTK adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/snippet_review.h"
#include "umicom/ui/gtk4/text_comparison.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include "umicom/developer_project/snippet_storage.h"
typedef struct SnippetStorageWork
{
    GWeakRef window;
    UmiEditorSnippetTemplate snippet;
    char path[UMI_PATH_CAPACITY];
    UmiStatus status;
    int save, stale;
} SnippetStorageWork;
typedef struct SnippetPanel
{
    GObject *lifetime;
    GPtrArray *controls;
    UmiDocumentSnippetReview *review;
    GtkWidget *body, *choice, *value, *expanded, *comparison, *approve, *apply, *message;
    gchar *template_body;
    gchar *application_directory, *base_override;
    GtkWidget *template_name, *save_template, *load_template;
    SnippetStorageWork *storage_pending;
    gulong body_changed_handler;
    char storage_language[UMI_EDITOR_SNIPPET_LANGUAGE_CAPACITY];
    uint64_t generation, presented;
    int closed, busy;
} SnippetPanel;
static SnippetPanel *SnippetPanelState(GtkWidget *window)
{
    return g_object_get_data(G_OBJECT(window), "umicom-document-snippet");
}
static UmiGtk4Adapter *SnippetPanelOwner(GtkWidget *window, SnippetPanel *panel)
{
    if (panel == NULL || panel->closed || g_object_get_data(panel->lifetime, "snippet-review") != window)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(panel->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void SnippetPanelFree(gpointer data)
{
    SnippetPanel *panel = data;
    UmiDocumentSnippetReviewDestroy(panel->review);
    g_free(panel->template_body);
    g_free(panel->application_directory);
    g_free(panel->base_override);
    g_ptr_array_unref(panel->controls);
    g_object_unref(panel->lifetime);
    g_free(panel);
}
static GtkWidget *SnippetPanelControl(SnippetPanel *panel, GtkWidget *box, GtkWidget *control, const char *id)
{
    g_ptr_array_add(panel->controls, g_object_ref_sink(control));
    (void)umi_gtk4_automation_tag_widget(control, id);
    gtk_box_append(GTK_BOX(box), control);
    return control;
}
static gchar *SnippetPanelText(GtkWidget *view)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
}
static void SnippetPanelExpire(SnippetPanel *panel)
{
    panel->presented = 0U;
    if (panel->generation != UINT64_MAX)
        ++panel->generation;
    gtk_widget_set_sensitive(panel->apply, FALSE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), FALSE);
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->comparison)) != NULL)
        gtk_box_remove(GTK_BOX(panel->comparison), child);
}
static void SnippetReviewClose(GtkWidget *window, int notify, UmiStatus status)
{
    if (window == NULL)
        return;
    g_object_ref(window);
    SnippetPanel *panel = SnippetPanelState(window);
    if (panel != NULL && !panel->closed)
    {
        GObject *lifetime = g_object_ref(panel->lifetime);
        UmiGtk4Adapter *owner = SnippetPanelOwner(window, panel);
        panel->closed = 1;
        if (panel->storage_pending != NULL)
            panel->storage_pending->stale = 1;
        panel->presented = 0U;
        if (g_object_get_data(lifetime, "snippet-review") == window)
            g_object_set_data(lifetime, "snippet-review", NULL);
        gtk_window_destroy(GTK_WINDOW(window));
        if (notify && owner != NULL && g_object_get_data(lifetime, "adapter") == owner &&
            owner->window != NULL && UmiGtk4WindowIsOpen(owner->window))
            (void)EditFinished(owner, status);
        g_object_unref(lifetime);
    }
    g_object_unref(window);
}
static void SnippetReviewDetach(GObject *lifetime)
{
    SnippetReviewClose(g_object_get_data(lifetime, "snippet-review"), 0, UMI_STATUS_CANCELLED);
}
static gboolean SnippetPanelClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    SnippetReviewClose(GTK_WIDGET(window), 1, UMI_STATUS_CANCELLED);
    return TRUE;
}
static void SnippetPanelRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    SnippetReviewClose(window, 1, UMI_STATUS_CANCELLED);
}
static void SnippetPanelParentRemoved(GtkWidget *parent, gpointer data)
{
    (void)parent;
    SnippetReviewClose(GTK_WIDGET(data), 0, UMI_STATUS_CANCELLED);
}
static void SnippetPanelCancel(GtkButton *button, gpointer data)
{
    (void)button;
    SnippetReviewClose(GTK_WIDGET(data), 1, UMI_STATUS_CANCELLED);
}
static void SnippetPanelChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) != NULL)
    {
        if (panel->storage_pending != NULL)
            panel->storage_pending->stale = 1;
        SnippetPanelExpire(panel);
    }
    g_object_unref(window);
}
/* Map the visible choice to primary placeholders only. Repeated occurrences
 * remain in the model but never ask the user to enter the same value twice. */
static UmiStatus SnippetPanelSelected(SnippetPanel *panel, UmiEditorSnippetPlaceholder *out)
{
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->choice));
    if (selected == GTK_INVALID_LIST_POSITION)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentSnippetSummary summary;
    UmiStatus status = UmiDocumentSnippetReviewInspect(panel->review, &summary);
    size_t visible = 0U;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.placeholder_count; ++i)
    {
        UmiEditorSnippetPlaceholder placeholder;
        status = UmiDocumentSnippetReviewPlaceholder(panel->review, i, &placeholder);
        if (status == UMI_STATUS_OK && placeholder.primary && !placeholder.final_stop)
        {
            if (visible == (size_t)selected)
            {
                *out = placeholder;
                return UMI_STATUS_OK;
            }
            ++visible;
        }
    }
    return status == UMI_STATUS_OK ? UMI_STATUS_NOT_FOUND : status;
}
static void SnippetPanelChoose(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) != NULL && !panel->busy)
    {
        SnippetPanelExpire(panel);
        if (SnippetPanelOwner(window, panel) != NULL)
        {
            UmiEditorSnippetPlaceholder placeholder;
            UmiStatus status = SnippetPanelSelected(panel, &placeholder);
            /* Copy metadata is safe even if a native text observer closes the form. */
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->value)),
                                     status == UMI_STATUS_OK ? placeholder.default_text : "", -1);
        }
    }
    g_object_unref(window);
}
static int SnippetPanelInputsCurrent(SnippetPanel *panel)
{
    gchar *body = SnippetPanelText(panel->body), *value = SnippetPanelText(panel->value);
    UmiEditorSnippetPlaceholder placeholder;
    UmiStatus status = SnippetPanelSelected(panel, &placeholder);
    int current = panel->template_body != NULL && strcmp(body, panel->template_body) == 0 &&
                  (status == UMI_STATUS_OK ? strcmp(value, placeholder.default_text) == 0
                                           : status == UMI_STATUS_NOT_FOUND && value[0] == '\0');
    g_free(body);
    g_free(value);
    return current;
}
static void SnippetPanelStatus(GtkWidget *window, SnippetPanel *panel, UmiStatus status, const char *success)
{
    if (SnippetPanelOwner(window, panel) == NULL)
        return;
    gtk_label_set_text(GTK_LABEL(panel->message),
                       status == UMI_STATUS_OK
                           ? success
                           : "The operation could not be completed. Check the template and value limits, or "
                             "close this form and capture the current draft again.");
}
static void SnippetPanelExpand(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) == NULL || panel->busy)
    {
        g_object_unref(window);
        return;
    }
    panel->busy = 1;
    SnippetPanelExpire(panel);
    uint64_t generation = panel->generation;
    gchar *body = SnippetPanelText(panel->body);
    UmiEditorSnippetTemplate *snippet = g_new0(UmiEditorSnippetTemplate, 1);
    snippet->struct_size = (uint32_t)sizeof(*snippet);
    snippet->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    (void)snprintf(snippet->id, sizeof(snippet->id), "local-editor");
    (void)snprintf(snippet->language_id, sizeof(snippet->language_id), "plaintext");
    (void)snprintf(snippet->name, sizeof(snippet->name), "Local editor template");
    UmiDocumentSnippetSummary summary;
    UmiStatus status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strlen(body) < sizeof(snippet->body))
    {
        memcpy(snippet->body, body, strlen(body) + 1U);
        status = UmiDocumentSnippetReviewInspect(panel->review, &summary);
        if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL)
            status = UmiDocumentSnippetReviewTemplate(panel->review, summary.revision, snippet, NULL);
        else
            status = UMI_STATUS_CANCELLED;
    }
    g_free(snippet);
    GtkStringList *labels = gtk_string_list_new(NULL);
    gchar *expanded_copy = NULL;
    if (status == UMI_STATUS_OK)
    {
        g_free(panel->template_body);
        panel->template_body = g_strdup(body);
        status = UmiDocumentSnippetReviewInspect(panel->review, &summary);
        for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.placeholder_count; ++i)
        {
            UmiEditorSnippetPlaceholder placeholder;
            status = UmiDocumentSnippetReviewPlaceholder(panel->review, i, &placeholder);
            if (status == UMI_STATUS_OK && placeholder.primary && !placeholder.final_stop)
            {
                gchar *label = g_strdup_printf(
                    "Value %u%s%s", (unsigned)placeholder.ordinal,
                    placeholder.choices[0] == '\0' ? "" : " — choices: ", placeholder.choices);
                gtk_string_list_append(labels, label);
                g_free(label);
            }
        }
        const char *expanded = NULL;
        size_t bytes = 0U;
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSnippetReviewExpanded(panel->review, &expanded, &bytes);
        if (status == UMI_STATUS_OK)
            expanded_copy = g_strndup(expanded, bytes);
    }
    g_free(body);
    if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL &&
        panel->generation == generation)
    {
        gtk_drop_down_set_model(GTK_DROP_DOWN(panel->choice), G_LIST_MODEL(labels));
        if (SnippetPanelOwner(window, panel) != NULL && panel->generation == generation)
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->expanded)), expanded_copy,
                                     -1);
    }
    g_object_unref(labels);
    g_free(expanded_copy);
    panel->busy = 0;
    if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL &&
        panel->generation == generation)
        SnippetPanelChoose(NULL, NULL, window);
    SnippetPanelStatus(window, panel, status,
                       "Choose a numbered value, edit it, then Update linked value. Preview the complete "
                       "insertion before applying.");
    g_object_unref(window);
}
static void SnippetPanelUpdate(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) == NULL || panel->busy)
    {
        g_object_unref(window);
        return;
    }
    panel->busy = 1;
    SnippetPanelExpire(panel);
    uint64_t generation = panel->generation;
    gchar *value = SnippetPanelText(panel->value), *body = SnippetPanelText(panel->body);
    UmiEditorSnippetPlaceholder placeholder;
    UmiDocumentSnippetSummary summary;
    UmiStatus status = SnippetPanelSelected(panel, &placeholder);
    if (status == UMI_STATUS_OK && (panel->template_body == NULL || strcmp(body, panel->template_body) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewInspect(panel->review, &summary);
    if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL)
        status = UmiDocumentSnippetReviewValue(panel->review, summary.revision, placeholder.ordinal, value,
                                               strlen(value), NULL);
    else if (SnippetPanelOwner(window, panel) == NULL)
        status = UMI_STATUS_CANCELLED;
    const char *expanded = NULL;
    size_t bytes = 0U;
    gchar *copy = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewExpanded(panel->review, &expanded, &bytes);
    if (status == UMI_STATUS_OK)
        copy = g_strndup(expanded, bytes);
    if (copy != NULL && SnippetPanelOwner(window, panel) != NULL && panel->generation == generation)
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->expanded)), copy, -1);
    g_free(copy);
    g_free(value);
    g_free(body);
    panel->busy = 0;
    SnippetPanelStatus(window, panel, status,
                       "All linked occurrences were updated. Preview the complete insertion to review the "
                       "surrounding source.");
    g_object_unref(window);
}
static void SnippetPanelPreview(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) == NULL || panel->busy)
    {
        g_object_unref(window);
        return;
    }
    panel->busy = 1;
    SnippetPanelExpire(panel);
    uint64_t generation = panel->generation;
    UmiGtk4Adapter *owner = SnippetPanelOwner(window, panel);
    UmiDocumentSnippetSummary summary;
    UmiStatus status = owner != NULL && SnippetPanelInputsCurrent(panel)
                           ? UmiDocumentSnippetReviewInspect(panel->review, &summary)
                           : UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status =
            UmiDocumentSnippetReviewPrepare(owner->edit_coordinator, panel->review, summary.revision, NULL);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewInspect(panel->review, &summary);
    const char *original = NULL, *proposed = NULL;
    size_t original_bytes = 0U, proposed_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewOriginal(panel->review, &original, &original_bytes);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewProposed(panel->review, &proposed, &proposed_bytes);
    /* Native widget construction can notify application observers. Give the
     * comparison independent bytes before any such callback can retire a model. */
    gchar *left = status == UMI_STATUS_OK ? g_strndup(original, original_bytes) : NULL;
    gchar *right = status == UMI_STATUS_OK ? g_strndup(proposed, proposed_bytes) : NULL;
    GtkWidget *comparison = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiGtk4TextComparisonCreate(left, original_bytes, right, proposed_bytes,
                                             "Captured source draft", "Draft with snippet", &comparison);
    if (comparison != NULL)
        g_object_ref_sink(comparison);
    g_free(left);
    g_free(right);
    if (status == UMI_STATUS_OK && SnippetPanelOwner(window, panel) != NULL &&
        panel->generation == generation && generation != UINT64_MAX && SnippetPanelInputsCurrent(panel))
    {
        gtk_box_append(GTK_BOX(panel->comparison), comparison);
        if (SnippetPanelOwner(window, panel) != NULL && panel->generation == generation &&
            SnippetPanelInputsCurrent(panel))
            panel->presented = summary.revision;
    }
    g_clear_object(&comparison);
    panel->busy = 0;
    SnippetPanelStatus(
        window, panel, status,
        "Review the complete draft, approve it, then apply. The file is not saved automatically.");
    g_object_unref(window);
}
static void SnippetPanelApproval(GtkCheckButton *button, gpointer data)
{
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    if (SnippetPanelOwner(window, panel) != NULL)
        gtk_widget_set_sensitive(panel->apply, !panel->busy && panel->presented != 0U &&
                                                   gtk_check_button_get_active(button) &&
                                                   SnippetPanelInputsCurrent(panel));
    g_object_unref(window);
}
static void SnippetPanelApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    SnippetPanel *panel = SnippetPanelState(window);
    UmiGtk4Adapter *owner = SnippetPanelOwner(window, panel);
    if (owner != NULL && !panel->busy && panel->presented != 0U &&
        gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)) && SnippetPanelInputsCurrent(panel))
    {
        panel->busy = 1;
        UmiStatus status =
            UmiDocumentSnippetReviewApply(owner->edit_coordinator, panel->review, panel->presented, 1);
        panel->busy = 0;
        if (status == UMI_STATUS_OK)
            SnippetReviewClose(window, 1, status);
        else
        {
            SnippetPanelExpire(panel);
            SnippetPanelStatus(window, panel, status, "");
        }
    }
    g_object_unref(window);
}
#include "document_snippet_storage.inc"
/* Named template persistence belongs to the shared adapter; the original local
 * form is retained for review and remains available through its public wrapper. */
#if 0
UmiStatus UmiGtk4AdapterReviewSnippet(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterCompletionReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || adapter->edit_cancel != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentId id = 0U;
    UmiDocumentSnippetReview *review = NULL;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewCreate(adapter->edit_coordinator, id, &review);
    if (status != UMI_STATUS_OK)
        return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    SnippetPanel *panel = g_new0(SnippetPanel, 1);
    panel->review = review;
    panel->generation = 1U;
    panel->lifetime = g_object_ref(lifetime);
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    g_object_set_data_full(G_OBJECT(window), "umicom-document-snippet", panel, SnippetPanelFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    GtkWidget *help = SnippetPanelControl(
        panel, box,
        gtk_label_new("Write a local template using ${1:name}, repeat $1 to link its value, and use $0 for "
                      "the final caret. Choices use ${2|one,two|}. Nested placeholders, variables, "
                      "transforms and escape sequences are not supported. Expand before editing values."),
        "document.snippet.help");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    panel->body = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.template");
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->body), TRUE);
    gtk_widget_set_size_request(panel->body, -1, 100);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->body)), "${1:text}$0", -1);
    GtkWidget *expand = SnippetPanelControl(panel, box, gtk_button_new_with_label("Expand template"),
                                            "document.snippet.expand");
    panel->choice = SnippetPanelControl(panel, box, gtk_drop_down_new(NULL, NULL), "document.snippet.choice");
    panel->value = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.value");
    gtk_widget_set_size_request(panel->value, -1, 70);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->value), TRUE);
    GtkWidget *update = SnippetPanelControl(panel, box, gtk_button_new_with_label("Update linked value"),
                                            "document.snippet.update");
    panel->expanded = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.expanded");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->expanded), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->expanded), TRUE);
    GtkWidget *preview = SnippetPanelControl(
        panel, box, gtk_button_new_with_label("Preview complete insertion"), "document.snippet.preview");
    panel->comparison = SnippetPanelControl(panel, box, gtk_box_new(GTK_ORIENTATION_VERTICAL, 4),
                                            "document.snippet.comparison");
    panel->approve =
        SnippetPanelControl(panel, box, gtk_check_button_new_with_label("I approve the displayed insertion"),
                            "document.snippet.approve");
    panel->apply = SnippetPanelControl(panel, box, gtk_button_new_with_label("Apply to captured draft"),
                                       "document.snippet.apply");
    GtkWidget *cancel =
        SnippetPanelControl(panel, box, gtk_button_new_with_label("Cancel"), "document.snippet.cancel");
    panel->message = SnippetPanelControl(
        panel, box,
        gtk_label_new(
            "This form works locally. It does not contact a language server or save your source file."),
        "document.snippet.message");
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    gtk_widget_set_sensitive(panel->apply, FALSE);
    (void)umi_gtk4_automation_tag_widget(window, "document.snippet.review");
    (void)UmiGtk4RecordingSetPrivate(window, 1);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->body)), "changed",
                            G_CALLBACK(SnippetPanelChanged), window, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->value)), "changed",
                            G_CALLBACK(SnippetPanelChanged), window, 0);
    g_signal_connect_object(panel->choice, "notify::selected", G_CALLBACK(SnippetPanelChoose), window, 0);
    g_signal_connect_object(expand, "clicked", G_CALLBACK(SnippetPanelExpand), window, 0);
    g_signal_connect_object(update, "clicked", G_CALLBACK(SnippetPanelUpdate), window, 0);
    g_signal_connect_object(preview, "clicked", G_CALLBACK(SnippetPanelPreview), window, 0);
    g_signal_connect_object(panel->approve, "toggled", G_CALLBACK(SnippetPanelApproval), window, 0);
    g_signal_connect_object(panel->apply, "clicked", G_CALLBACK(SnippetPanelApply), window, 0);
    g_signal_connect_object(cancel, "clicked", G_CALLBACK(SnippetPanelCancel), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(SnippetPanelClosing), NULL);
    g_signal_connect(window, "unmap", G_CALLBACK(SnippetPanelRemoved), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(SnippetPanelRemoved), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "Review snippet insertion");
    gtk_window_set_default_size(GTK_WINDOW(window), 920, 740);
    gtk_window_set_default_widget(GTK_WINDOW(window), cancel);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    else if (UmiGtk4AdapterCompletionReviewBusy(adapter))
        status = UMI_STATUS_BUSY;
    else
    {
        g_object_set_data(lifetime, "snippet-review", window);
        /* Retained GTK windows can leave the native toplevel list before their
         * destroy signal runs. Share the removal watch so those controls lose
         * document authority immediately, including an unpresented parent. */
        (void)UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), SnippetPanelRemoved, NULL);
        (void)UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), SnippetPanelParentRemoved, window);
        g_signal_connect_object(parent, "destroy", G_CALLBACK(SnippetPanelParentRemoved), window, 0);
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_window_present(GTK_WINDOW(window));
        if (SnippetPanelOwner(window, panel) == NULL)
            status = UMI_STATUS_CANCELLED;
    }
    if (status != UMI_STATUS_OK)
        SnippetReviewClose(window, 0, status);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
#endif
static UmiStatus SnippetPanelCreate(UmiGtk4Adapter *adapter, const char *application_directory,
                                    const char *base_override)
{
    if (adapter == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL || adapter->window == NULL ||
        !UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_UNAVAILABLE;
    if (UmiGtk4AdapterCompletionReviewBusy(adapter) || UmiGtk4AdapterDocumentProposalBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || adapter->edit_cancel != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentId id = 0U;
    UmiDocumentSnippetReview *review = NULL;
    UmiStatus status = EditFindDocument(adapter, NULL, &id);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSnippetReviewCreate(adapter->edit_coordinator, id, &review);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentSnippetSummary captured = {0};
    status = UmiDocumentSnippetReviewInspect(review, &captured);
    const char *language = captured.source.language_id[0] == '\0' ? "plaintext" : captured.source.language_id;
    char validated_path[UMI_PATH_CAPACITY];
    if (status == UMI_STATUS_OK && application_directory != NULL)
        status = UmiSnippetTemplatePath(application_directory, base_override, language, "validation",
                                        validated_path, sizeof(validated_path));
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentSnippetReviewDestroy(review);
        return status;
    }
    gchar *application_copy = g_strdup(application_directory), *base_copy = g_strdup(base_override);
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    GtkWidget *window = gtk_window_new();
    g_object_ref(window);
    SnippetPanel *panel = g_new0(SnippetPanel, 1);
    panel->review = review;
    panel->application_directory = application_copy;
    panel->base_override = base_copy;
    if (application_directory != NULL)
        memcpy(panel->storage_language, language, strlen(language) + 1U);
    panel->generation = 1U;
    panel->lifetime = g_object_ref(lifetime);
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    g_object_set_data_full(G_OBJECT(window), "umicom-document-snippet", panel, SnippetPanelFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    gtk_window_set_child(GTK_WINDOW(window), scroll);
    GtkWidget *help = SnippetPanelControl(
        panel, box,
        gtk_label_new("Write a local template using ${1:name}, repeat $1 to link its value, and use $0 for "
                      "the final caret. Choices use ${2|one,two|}. Nested placeholders, variables, "
                      "transforms and escape sequences are not supported. Expand before editing values."),
        "document.snippet.help");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    panel->body = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.template");
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->body), TRUE);
    gtk_widget_set_size_request(panel->body, -1, 100);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->body)), "${1:text}$0", -1);
    if (application_directory != NULL)
    {
        GtkWidget *label =
            SnippetPanelControl(panel, box,
                                gtk_label_new("Template name — Save replaces this name for the current "
                                              "language; Load changes this form only."),
                                "document.snippet.storage.help");
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        panel->template_name = SnippetPanelControl(panel, box, gtk_entry_new(), "document.snippet.name");
        gtk_entry_set_placeholder_text(GTK_ENTRY(panel->template_name), "For example: loop or error check");
        panel->save_template = SnippetPanelControl(
            panel, box, gtk_button_new_with_label("Save named template"), "document.snippet.save");
        panel->load_template = SnippetPanelControl(
            panel, box, gtk_button_new_with_label("Load named template"), "document.snippet.load");
        g_signal_connect_object(panel->template_name, "changed", G_CALLBACK(SnippetStorageNameChanged),
                                window, 0);
        g_signal_connect_object(panel->save_template, "clicked", G_CALLBACK(SnippetStorageSave), window, 0);
        g_signal_connect_object(panel->load_template, "clicked", G_CALLBACK(SnippetStorageLoad), window, 0);
    }
    GtkWidget *expand = SnippetPanelControl(panel, box, gtk_button_new_with_label("Expand template"),
                                            "document.snippet.expand");
    panel->choice = SnippetPanelControl(panel, box, gtk_drop_down_new(NULL, NULL), "document.snippet.choice");
    panel->value = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.value");
    gtk_widget_set_size_request(panel->value, -1, 70);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->value), TRUE);
    GtkWidget *update = SnippetPanelControl(panel, box, gtk_button_new_with_label("Update linked value"),
                                            "document.snippet.update");
    panel->expanded = SnippetPanelControl(panel, box, gtk_text_view_new(), "document.snippet.expanded");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->expanded), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->expanded), TRUE);
    GtkWidget *preview = SnippetPanelControl(
        panel, box, gtk_button_new_with_label("Preview complete insertion"), "document.snippet.preview");
    panel->comparison = SnippetPanelControl(panel, box, gtk_box_new(GTK_ORIENTATION_VERTICAL, 4),
                                            "document.snippet.comparison");
    panel->approve =
        SnippetPanelControl(panel, box, gtk_check_button_new_with_label("I approve the displayed insertion"),
                            "document.snippet.approve");
    panel->apply = SnippetPanelControl(panel, box, gtk_button_new_with_label("Apply to captured draft"),
                                       "document.snippet.apply");
    GtkWidget *cancel =
        SnippetPanelControl(panel, box, gtk_button_new_with_label("Cancel"), "document.snippet.cancel");
    panel->message = SnippetPanelControl(
        panel, box,
        gtk_label_new(
            "This form works locally. It does not contact a language server or save your source file."),
        "document.snippet.message");
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    gtk_widget_set_sensitive(panel->apply, FALSE);
    (void)umi_gtk4_automation_tag_widget(window, "document.snippet.review");
    (void)UmiGtk4RecordingSetPrivate(window, 1);
    panel->body_changed_handler =
        g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->body)), "changed",
                                G_CALLBACK(SnippetPanelChanged), window, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->value)), "changed",
                            G_CALLBACK(SnippetPanelChanged), window, 0);
    g_signal_connect_object(panel->choice, "notify::selected", G_CALLBACK(SnippetPanelChoose), window, 0);
    g_signal_connect_object(expand, "clicked", G_CALLBACK(SnippetPanelExpand), window, 0);
    g_signal_connect_object(update, "clicked", G_CALLBACK(SnippetPanelUpdate), window, 0);
    g_signal_connect_object(preview, "clicked", G_CALLBACK(SnippetPanelPreview), window, 0);
    g_signal_connect_object(panel->approve, "toggled", G_CALLBACK(SnippetPanelApproval), window, 0);
    g_signal_connect_object(panel->apply, "clicked", G_CALLBACK(SnippetPanelApply), window, 0);
    g_signal_connect_object(cancel, "clicked", G_CALLBACK(SnippetPanelCancel), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(SnippetPanelClosing), NULL);
    g_signal_connect(window, "unmap", G_CALLBACK(SnippetPanelRemoved), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(SnippetPanelRemoved), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "Review snippet insertion");
    gtk_window_set_default_size(GTK_WINDOW(window), 920, 740);
    gtk_window_set_default_widget(GTK_WINDOW(window), cancel);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    else if (UmiGtk4AdapterCompletionReviewBusy(adapter))
        status = UMI_STATUS_BUSY;
    else
    {
        g_object_set_data(lifetime, "snippet-review", window);
        /* Retained GTK windows can leave the native toplevel list before their
         * destroy signal runs. Share the removal watch so those controls lose
         * document authority immediately, including an unpresented parent. */
        (void)UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), SnippetPanelRemoved, NULL);
        (void)UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), SnippetPanelParentRemoved, window);
        g_signal_connect_object(parent, "destroy", G_CALLBACK(SnippetPanelParentRemoved), window, 0);
        if (SnippetPanelOwner(window, panel) != NULL)
            gtk_window_present(GTK_WINDOW(window));
        if (SnippetPanelOwner(window, panel) == NULL)
            status = UMI_STATUS_CANCELLED;
    }
    if (status != UMI_STATUS_OK)
        SnippetReviewClose(window, 0, status);
    g_object_unref(parent);
    g_object_unref(lifetime);
    g_object_unref(window);
    return status;
}
UmiStatus UmiGtk4AdapterReviewSnippet(UmiGtk4Adapter *adapter)
{
    return SnippetPanelCreate(adapter, NULL, NULL);
}
UmiStatus UmiGtk4AdapterReviewSnippetWithSettings(UmiGtk4Adapter *adapter, const char *application_directory,
                                                  const char *base_override)
{
    if (application_directory == NULL || application_directory[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return SnippetPanelCreate(adapter, application_directory, base_override);
}
