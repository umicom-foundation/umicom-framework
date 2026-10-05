/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_connections_context_gtk4.c
 * PURPOSE: Own an explicit selection draft until it is handed to one chat window.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "provider_connections_internal.h"
#include "provider_chat_context_internal.h"
#include "umicom/security/secrets.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <string.h>

UmiStatus UmiProviderConnectionsGtkSetChatContext(UmiProviderConnectionsGtk *panel, const char *text)
{
    if (panel == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (panel->closed)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiProviderChatGtkContextValidate(text);
    if (status != UMI_STATUS_OK)
        return status;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->chat_context)), text, -1);
    char *summary =
        text[0] != '\0'
            ? g_strdup_printf("Captured context: %zu bytes. This is a copy; later editor changes are not "
                              "included. Choose a saved connection, then open chat to edit and review it.",
                              strlen(text))
            : g_strdup("No captured context. Opening chat starts with an empty context field.");
    gtk_label_set_text(GTK_LABEL(panel->chat_context_summary), summary);
    g_free(summary);
    return UMI_STATUS_OK;
}
static void Clear(GtkButton *button, gpointer root)
{
    (void)button;
    UmiProviderConnectionsGtk *panel = g_object_get_data(G_OBJECT(root), "umicom-provider-editor");
    if (panel != NULL && !panel->closed)
        (void)UmiProviderConnectionsGtkSetChatContext(panel, "");
}
void UmiProviderChatContextControls(UmiProviderConnectionsGtk *panel, GtkWidget *box)
{
    panel->chat_context_summary =
        gtk_label_new("No captured context. Opening chat starts with an empty context field.");
    gtk_label_set_wrap(GTK_LABEL(panel->chat_context_summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->chat_context_summary), 0.0F);
    gtk_box_append(GTK_BOX(box), panel->chat_context_summary);
    panel->chat_context = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->chat_context), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->chat_context), GTK_WRAP_WORD_CHAR);
    gtk_accessible_update_property(GTK_ACCESSIBLE(panel->chat_context), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                   "Captured context for one chat", -1);
    (void)UmiGtk4RecordingSetPrivate(panel->chat_context, 1);
    (void)umi_gtk4_automation_tag_widget(panel->chat_context, "connections.chat-context");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_size_request(scroll, -1, 100);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->chat_context);
    gtk_box_append(GTK_BOX(box), scroll);
    GtkWidget *clear = gtk_button_new_with_label("Clear captured context");
    (void)umi_gtk4_automation_tag_widget(clear, "connections.clear-chat-context");
    g_signal_connect_object(clear, "clicked", G_CALLBACK(Clear), panel->root, 0);
    gtk_box_append(GTK_BOX(box), clear);
}
UmiStatus UmiProviderChatContextOpen(UmiProviderConnectionsGtk *panel, GtkWindow *parent,
                                     const UmiProviderChatGtkConfig *config)
{
    if (panel == NULL || panel->closed)
        return UMI_STATUS_INVALID_STATE;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->chat_context));
    GtkTextIter start, end;
    if ((size_t)gtk_text_buffer_get_char_count(buffer) >= UMI_PROVIDER_CHAT_CONTEXT_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    /* The chat window owns a new copy before this temporary buffer is cleared.
     * A failed open preserves the displayed draft for correction or retry. */
    UmiStatus status = UmiProviderChatGtkPresentWithContext(parent, config, text);
    umi_secret_clear(text, strlen(text));
    g_free(text);
    if (status == UMI_STATUS_OK)
        (void)UmiProviderConnectionsGtkSetChatContext(panel, "");
    return status;
}
void UmiProviderChatContextRetire(UmiProviderConnectionsGtk *panel)
{
    /* A retained widget must not keep the selected code after retirement. */
    if (panel->chat_context != NULL)
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->chat_context)), "", -1);
}
