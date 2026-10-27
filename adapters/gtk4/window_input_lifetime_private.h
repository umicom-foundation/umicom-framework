/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/window_input_lifetime_private.h
 *
 * PURPOSE:
 *   Release text-input focus before a native window loses its surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_WINDOW_INPUT_LIFETIME_PRIVATE_H
#define UMICOM_GTK4_WINDOW_INPUT_LIFETIME_PRIVATE_H
#include <gtk/gtk.h>

/* Input methods may own native message callbacks while a text widget has focus.
 * Release that focus before the window's unrealize class handler removes the
 * native surface. Clearing it after child disposal is too late for backends
 * which need the surface to unregister their callbacks.
 *
 * This uses GTK's normal focus-out lifecycle; it neither disables input methods
 * nor changes application close/discard policy. Call only on the GTK thread. */
static void UmiGtk4WindowInputUnrealize(GtkWidget *widget, gpointer unused)
{
    (void)unused;
    if (g_object_get_data(G_OBJECT(widget), "umicom-input-unrealizing") != NULL)
        return;
    g_object_ref(widget);
    g_object_set_data(G_OBJECT(widget), "umicom-input-unrealizing", GINT_TO_POINTER(1));
    gtk_window_set_focus(GTK_WINDOW(widget), NULL);
    g_object_set_data(G_OBJECT(widget), "umicom-input-unrealizing", NULL);
    g_object_unref(widget);
}

static inline void UmiGtk4PrepareWindowInputLifetime(GtkWindow *window)
{
    g_return_if_fail(GTK_IS_WINDOW(window));
    if (g_object_get_data(G_OBJECT(window), "umicom-input-lifetime") != NULL)
        return;
    g_object_set_data(G_OBJECT(window), "umicom-input-lifetime", GINT_TO_POINTER(1));
    /* GtkWidget::unrealize is RUN_LAST. An ordinary handler runs while the
     * window and its text-input widgets still have their native context. */
    g_signal_connect(window, "unrealize", G_CALLBACK(UmiGtk4WindowInputUnrealize), NULL);
}
#endif
