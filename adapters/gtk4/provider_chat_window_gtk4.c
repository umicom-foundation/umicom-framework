/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/provider_chat_window_gtk4.c
 *
 * PURPOSE:
 *   Give selected-connection chat a transient window whose controller retires on unrealize.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "window_input_lifetime_private.h"
#include "umicom/provider_connections/chat_gtk4.h"
static void Release(gpointer data) { UmiProviderChatGtkDestroy(data); }
static void Retire(GtkWidget *window, gpointer data)
{
    (void)data;
    g_object_set_data(G_OBJECT(window), "umicom-chat-panel", NULL);
}
static gboolean Close(GtkWindow *window, gpointer data)
{
    (void)data;
    return !UmiProviderChatGtkCanClose(g_object_get_data(G_OBJECT(window), "umicom-chat-panel"));
}
/* Window creation now accepts copied context so each caller uses the same lifetime and review boundary. Empty-context callers retain their existing route.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiProviderChatGtkPresent(GtkWindow *parent, const UmiProviderChatGtkConfig *config)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderChatGtk *panel = NULL;
    UmiStatus status = UmiProviderChatGtkCreate(config, &panel);
    if (status != UMI_STATUS_OK)
        return status;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Chat with saved connection");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 800);
    if (parent != NULL)
    {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProviderChatGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-chat-panel", panel, Release);
    g_signal_connect(window, "unrealize", G_CALLBACK(Retire), NULL);
    g_signal_connect(window, "close-request", G_CALLBACK(Close), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiProviderChatGtkPresentWithContext(GtkWindow *parent, const UmiProviderChatGtkConfig *config, const char *context)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderChatGtk *panel = NULL;
    UmiStatus status = UmiProviderChatGtkCreate(config, &panel);
    if (status != UMI_STATUS_OK)
        return status;
    /* Preserve the ordinary empty window's clean-close behaviour. */
    status = context == NULL ? UMI_STATUS_INVALID_ARGUMENT
        : context[0] != '\0' ? UmiProviderChatGtkSetContext(panel, context) : UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) { UmiProviderChatGtkDestroy(panel); return status; }
    /* Empty legacy chat remains a clean window; a captured context requires
     * explicit discard when closing without sending. */
    GtkWidget *window = gtk_window_new();
    UmiGtk4PrepareWindowInputLifetime(GTK_WINDOW(window));
    gtk_window_set_title(GTK_WINDOW(window), "Chat with saved connection");
    gtk_window_set_default_size(GTK_WINDOW(window), 760, 800);
    if (parent != NULL)
    {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProviderChatGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-chat-panel", panel, Release);
/* Retire controllers after GTK unrealizes the child tree so text input contexts remain alive through native surface teardown. The former implementation is retained for review. */
#if 0
    g_signal_connect(window, "unrealize", G_CALLBACK(Retire), NULL);
#endif
    g_signal_connect_after(window, "unrealize", G_CALLBACK(Retire), NULL);
    g_signal_connect(window, "close-request", G_CALLBACK(Close), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderChatGtkPresent(GtkWindow *parent, const UmiProviderChatGtkConfig *config)
{
    return UmiProviderChatGtkPresentWithContext(parent, config, "");
}

