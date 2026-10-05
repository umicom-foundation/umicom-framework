/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_history_gtk4.c
 * PURPOSE: Let users select prior text explicitly without implicit conversation replay.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_chat_internal.h"
#include "umicom/security/secrets.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <inttypes.h>
#include <string.h>
static UmiProviderChatGtk *Panel(gpointer root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-provider-chat");
}
static const UmiProviderChatHistoryEntry *Selected(UmiProviderChatGtk *panel)
{
    return UmiProviderChatHistoryAt(panel->history,
                                    gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->history_selector)));
}
static void SelectionChanged(GObject *object, GParamSpec *property, gpointer root)
{
    (void)object;
    (void)property;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy || panel->history_painting)
        return;
    const UmiProviderChatHistoryEntry *entry = Selected(panel);
    panel->history_revision = UmiProviderChatHistoryRevision(panel->history);
    panel->history_entry = entry != NULL ? entry->id : 0U;
    panel->history_role = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->history_part)) == 0U
                              ? UMI_AI_ROLE_USER
                              : UMI_AI_ROLE_ASSISTANT;
    UmiProviderChatGtkText(panel->history_view, entry == NULL                             ? ""
                                                : panel->history_role == UMI_AI_ROLE_USER ? entry->request
                                                                                          : entry->reply);
    if (entry == NULL)
    {
        gtk_label_set_text(GTK_LABEL(panel->history_detail), "No earlier exchange selected.");
        return;
    }
    char *detail = g_strdup_printf(
        "Exchange %" PRIu64 " · connection %s at revision %" PRIu64
        "\n%s\nRequested model: %s · reported model: %s\n%s%sSelect only the text you want to include next.",
        entry->id, entry->connection, entry->connection_revision, entry->endpoint, entry->requested_model,
        entry->reported_model, entry->refused ? "Provider refusal. " : "",
        entry->truncated ? "Reply is incomplete. " : "");
    gtk_label_set_text(GTK_LABEL(panel->history_detail), detail);
    g_free(detail);
}
void UmiProviderChatGtkHistoryRefresh(UmiProviderChatGtk *panel, uint64_t selected_id)
{
    if (panel->closed)
        return;
    panel->history_painting = true;
    GtkStringList *rows = gtk_string_list_new(NULL);
    guint selected = GTK_INVALID_LIST_POSITION;
    for (size_t i = 0U; i < UmiProviderChatHistoryCount(panel->history); ++i)
    {
        const UmiProviderChatHistoryEntry *entry = UmiProviderChatHistoryAt(panel->history, i);
        char *label =
            g_strdup_printf("Exchange %" PRIu64 " · %s%s%s", entry->id, entry->requested_model,
                            entry->refused ? " · refusal" : "", entry->truncated ? " · incomplete" : "");
        gtk_string_list_append(rows, label);
        g_free(label);
        if (entry->id == selected_id)
            selected = (guint)i;
    }
    /* set_model borrows the list and takes its own reference. Unlike dropdown
     * construction, this setter does not consume the caller's reference. */
    gtk_drop_down_set_model(GTK_DROP_DOWN(panel->history_selector), G_LIST_MODEL(rows));
    g_object_unref(rows);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->history_selector), selected);
    panel->history_painting = false;
    SelectionChanged(NULL, NULL, panel->root);
}
static void UseExcerpt(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy)
        return;
    const UmiProviderChatHistoryEntry *entry = Selected(panel);
    if (entry == NULL || entry->id != panel->history_entry ||
        panel->history_revision != UmiProviderChatHistoryRevision(panel->history))
    {
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "History changed. Select the earlier exchange again.");
        return;
    }
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->history_view));
    GtkTextIter start, end;
    if (!gtk_text_buffer_get_selection_bounds(buffer, &start, &end))
    {
        gtk_label_set_text(
            GTK_LABEL(panel->history_message),
            "Highlight the exact earlier text to include. No complete message is added automatically.");
        return;
    }
    const char *source = panel->history_role == UMI_AI_ROLE_USER ? entry->request : entry->reply;
    gint first = gtk_text_iter_get_offset(&start), last = gtk_text_iter_get_offset(&end);
    glong characters = g_utf8_strlen(source, -1);
    if (first < 0 || last <= first || (glong)last > characters)
    {
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "Selection no longer matches this exchange. Select the earlier text again.");
        return;
    }
    /* Compare the rendered buffer with the immutable selected entry before
     * interpreting character coordinates. A stale or externally edited view
     * must not select different source bytes under an old approval. */
    GtkTextIter begin, finish;
    gtk_text_buffer_get_bounds(buffer, &begin, &finish);
    if (gtk_text_buffer_get_char_count(buffer) != characters)
    {
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "History display changed. Reselect the exchange before copying context.");
        return;
    }
    char *shown = gtk_text_buffer_get_text(buffer, &begin, &finish, TRUE);
    bool same = strcmp(shown, source) == 0;
    umi_secret_clear(shown, strlen(shown));
    g_free(shown);
    if (!same)
    {
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "History display changed. Reselect the exchange before copying context.");
        return;
    }
    const char *from = g_utf8_offset_to_pointer(source, first), *to = g_utf8_offset_to_pointer(source, last);
    UmiProviderChatHistoryExcerpt excerpt = {entry->id, panel->history_role, (size_t)(from - source),
                                             (size_t)(to - from)};
    char *context = g_try_malloc0(UMI_PROVIDER_CHAT_CONTEXT_CAPACITY);
    if (context == NULL)
    {
        gtk_label_set_text(GTK_LABEL(panel->history_message), "Not enough memory to prepare context.");
        return;
    }
    UmiStatus status = UmiProviderChatHistoryCompose(panel->history, panel->history_revision, &excerpt, 1U,
                                                     context, UMI_PROVIDER_CHAT_CONTEXT_CAPACITY);
    if (status == UMI_STATUS_OK)
        status = UmiProviderChatGtkSetContext(panel, context);
    umi_secret_clear(context, UMI_PROVIDER_CHAT_CONTEXT_CAPACITY);
    g_free(context);
    gtk_label_set_text(GTK_LABEL(panel->history_message),
                       status == UMI_STATUS_OK ? "Context replaced with the selected excerpt. Enter the next "
                                                 "prompt and review the complete request again."
                                               : "The excerpt could not fit the context limit, or history "
                                                 "changed. Choose less text and review again.");
}
static void Remove(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy)
        return;
    UmiStatus status =
        UmiProviderChatHistoryRemove(panel->history, panel->history_revision, panel->history_entry);
    if (status == UMI_STATUS_OK)
        UmiProviderChatGtkHistoryRefresh(panel, 0U);
    gtk_label_set_text(GTK_LABEL(panel->history_message),
                       status == UMI_STATUS_OK
                           ? "Exchange forgotten. Text already copied into your draft remains there; Clear "
                             "local text clears both draft and history."
                           : "Select a current exchange before forgetting it.");
}
static void ClearHistory(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderChatGtk *panel = Panel(root);
    if (panel == NULL || panel->closed || panel->busy)
        return;
    UmiStatus status = UmiProviderChatGtkHistoryClear(panel);
    gtk_label_set_text(GTK_LABEL(panel->history_message),
                       status == UMI_STATUS_OK
                           ? "Conversation cleared. Existing draft copies remain; Clear local text clears "
                             "everything in this window."
                           : "Conversation could not clear. Wait for the current operation and try again.");
}
UmiStatus UmiProviderChatGtkHistoryClear(UmiProviderChatGtk *panel)
{
    UmiStatus status =
        UmiProviderChatHistoryClear(panel->history, UmiProviderChatHistoryRevision(panel->history));
    if (status == UMI_STATUS_OK)
        UmiProviderChatGtkHistoryRefresh(panel, 0U);
    return status;
}
void UmiProviderChatGtkHistoryRecord(UmiProviderChatGtk *panel, const UmiProviderChatGtkJob *job)
{
    if (panel->closed || !job->send || job->status != UMI_STATUS_OK)
        return;
    uint64_t id = 0U;
    UmiStatus status = UmiProviderChatHistoryAppend(panel->history, job->plan, &job->result, &id);
    if (status == UMI_STATUS_OK)
    {
        UmiProviderChatGtkHistoryRefresh(panel, id);
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "Exchange kept in this window only. No earlier text is included in your next "
                           "request automatically.");
    }
    else
        gtk_label_set_text(GTK_LABEL(panel->history_message),
                           "Reply received but not added to local history. The history may be full; forget "
                           "an exchange or clear it. Do not resend just to record the reply.");
}
void UmiProviderChatGtkHistoryRetire(UmiProviderChatGtk *panel)
{
    UmiProviderChatGtkText(panel->history_view, "");
    gtk_label_set_text(GTK_LABEL(panel->history_detail), "");
    UmiProviderChatHistoryDestroy(panel->history);
    panel->history = NULL;
}
static GtkWidget *Button(UmiProviderChatGtk *panel, const char *text, const char *id, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(text);
    (void)umi_gtk4_automation_tag_widget(button, id);
    g_signal_connect_object(button, "clicked", callback, panel->root, 0);
    gtk_box_append(GTK_BOX(panel->history_box), button);
    return button;
}
void UmiProviderChatGtkHistoryControls(UmiProviderChatGtk *panel, GtkWidget *box)
{
    panel->history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(box), panel->history_box);
    GtkWidget *title = gtk_label_new("This window's conversation — up to eight exchanges, not saved to disk");
    gtk_label_set_wrap(GTK_LABEL(title), TRUE);
    gtk_box_append(GTK_BOX(panel->history_box), title);
    const char *empty[] = {NULL}, *roles[] = {"Earlier request", "Earlier reply", NULL};
    panel->history_selector = gtk_drop_down_new_from_strings(empty);
    panel->history_part = gtk_drop_down_new_from_strings(roles);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->history_part), 1U);
    (void)umi_gtk4_automation_tag_widget(panel->history_selector, "provider-chat.history");
    (void)umi_gtk4_automation_tag_widget(panel->history_part, "provider-chat.history-part");
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->history_selector), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Earlier exchange", -1);
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->history_part), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Request or reply", -1);
    gtk_box_append(GTK_BOX(panel->history_box), panel->history_selector);
    gtk_box_append(GTK_BOX(panel->history_box), panel->history_part);
    g_signal_connect_object(panel->history_selector, "notify::selected", G_CALLBACK(SelectionChanged),
                            panel->root, 0);
    g_signal_connect_object(panel->history_part, "notify::selected", G_CALLBACK(SelectionChanged),
                            panel->root, 0);
    panel->history_detail = gtk_label_new("No earlier exchange selected.");
    gtk_label_set_wrap(GTK_LABEL(panel->history_detail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->history_detail), 0.0F);
    gtk_box_append(GTK_BOX(panel->history_box), panel->history_detail);
    panel->history_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->history_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->history_view), GTK_WRAP_WORD_CHAR);
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->history_view), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Select earlier text to include", -1);
    (void)UmiGtk4RecordingSetPrivate(panel->history_view, 1);
    (void)umi_gtk4_automation_tag_widget(panel->history_view, "provider-chat.history-text");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 150);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->history_view);
    gtk_box_append(GTK_BOX(panel->history_box), scroll);
    Button(panel, "Replace context with selected excerpt", "provider-chat.history-use",
           G_CALLBACK(UseExcerpt));
    Button(panel, "Forget selected exchange", "provider-chat.history-forget", G_CALLBACK(Remove));
    Button(panel, "Clear conversation", "provider-chat.history-clear", G_CALLBACK(ClearHistory));
    panel->history_message = gtk_label_new(
        "Earlier text is never sent automatically. Select an excerpt to replace the current context.");
    gtk_label_set_wrap(GTK_LABEL(panel->history_message), TRUE);
    gtk_box_append(GTK_BOX(panel->history_box), panel->history_message);
}
