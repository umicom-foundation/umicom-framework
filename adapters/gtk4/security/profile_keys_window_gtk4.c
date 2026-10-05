/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/security/profile_keys_window_gtk4.c
 * PURPOSE: Own a transient credential panel independently of its application host.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/security/gtk4/profile_keys.h"

static void ReleasePanel(gpointer data)
{
    UmiProfileKeysGtkDestroy(data);
}
static void RetirePanel(GtkWidget *window, gpointer data)
{
    (void)data;
    /* A retained destroyed window must not leave working secret controls. */
    g_object_set_data(G_OBJECT(window), "umicom-profile-key-panel", NULL);
}
static gboolean CloseRequested(GtkWindow *window, gpointer data)
{
    (void)data;
    return !UmiProfileKeysGtkCanClose(g_object_get_data(G_OBJECT(window), "umicom-profile-key-panel"));
}
UmiStatus UmiProfileKeysGtkPresent(GtkWindow *parent, const char *application_id, const char *profile_name)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProfileKeysGtk *panel = NULL;
    UmiStatus status = UmiProfileKeysGtkCreate(application_id, profile_name, &panel);
    if (status != UMI_STATUS_OK) return status;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Local provider keys");
    gtk_window_set_default_size(GTK_WINDOW(window), 660, 740);
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
        gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    }
    gtk_window_set_child(GTK_WINDOW(window), UmiProfileKeysGtkWidget(panel));
    g_object_set_data_full(G_OBJECT(window), "umicom-profile-key-panel", panel, ReleasePanel);
    g_signal_connect(window, "close-request", G_CALLBACK(CloseRequested), NULL);
    g_signal_connect(window, "unrealize", G_CALLBACK(RetirePanel), NULL);
    gtk_window_present(GTK_WINDOW(window));
    return UMI_STATUS_OK;
}
