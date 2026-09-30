/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/window_removal_private.h
 * PURPOSE: Observe native window removal while callers retain windows or controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK4_WINDOW_REMOVAL_PRIVATE_H
#define UMICOM_GTK4_WINDOW_REMOVAL_PRIVATE_H
#include <gtk/gtk.h>

/* GTK drops its toplevel reference at gtk_window_destroy(); retained objects
 * may emit destroy much later. Observe the public toplevel model instead.
 * All calls and callbacks belong to the GTK main thread. The receiver owns
 * callback data; its finalisation cancels observation without invoking it. */
typedef struct UmiGtk4RemovalWatch {
    GWeakRef window;
    GObject *receiver; /* Weak, paired with ReceiverGone. */
    gulong signal;
    void (*callback)(GtkWidget *, gpointer);
    gpointer data;
} UmiGtk4RemovalWatch;

static void UmiGtk4RemovalReceiverGone(gpointer data, GObject *receiver);
static void UmiGtk4RemovalFree(gpointer data, GClosure *closure)
{
    (void)closure;
    UmiGtk4RemovalWatch *watch = data;
    if (watch->receiver != NULL)
        g_object_weak_unref(watch->receiver, UmiGtk4RemovalReceiverGone, watch);
    g_weak_ref_clear(&watch->window);
    g_free(watch);
}
static void UmiGtk4RemovalReceiverGone(gpointer data, GObject *receiver)
{
    (void)receiver;
    UmiGtk4RemovalWatch *watch = data;
    watch->receiver = NULL;
    g_signal_handler_disconnect(gtk_window_get_toplevels(), watch->signal);
}
static gboolean UmiGtk4WindowRegistered(GtkWindow *window)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GObject *item = g_list_model_get_item(windows, i);
        gboolean found = item == G_OBJECT(window);
        g_object_unref(item);
        if (found) return TRUE;
    }
    return FALSE;
}
static void UmiGtk4RemovalChanged(GListModel *model, guint position,
    guint removed, guint added, gpointer data)
{
    (void)position; (void)removed; (void)added;
    UmiGtk4RemovalWatch *watch = data;
    GtkWindow *window = g_weak_ref_get(&watch->window);
    if (window != NULL && UmiGtk4WindowRegistered(window)) {
        g_object_unref(window);
        return;
    }
    /* Disconnect before invoking application code: callbacks may destroy
     * another window, release their receiver, or register another observer. */
    GObject *receiver = g_object_ref(watch->receiver);
    void (*callback)(GtkWidget *, gpointer) = watch->callback;
    gpointer callbackData = watch->data;
    g_object_weak_unref(receiver, UmiGtk4RemovalReceiverGone, watch);
    watch->receiver = NULL;
    g_signal_handler_disconnect(model, watch->signal);
    if (window != NULL) callback(GTK_WIDGET(window), callbackData);
    g_clear_object(&window);
    g_object_unref(receiver);
}
static inline void UmiGtk4ObserveWindowRemoval(GtkWindow *window,
    GObject *receiver, void (*callback)(GtkWidget *, gpointer), gpointer data)
{
    g_return_if_fail(GTK_IS_WINDOW(window) && G_IS_OBJECT(receiver) && callback != NULL);
    if (!UmiGtk4WindowRegistered(window)) {
        callback(GTK_WIDGET(window), data);
        return;
    }
    UmiGtk4RemovalWatch *watch = g_new0(UmiGtk4RemovalWatch, 1);
    g_weak_ref_init(&watch->window, window);
    watch->receiver = receiver; watch->callback = callback; watch->data = data;
    watch->signal = g_signal_connect_data(gtk_window_get_toplevels(), "items-changed",
        G_CALLBACK(UmiGtk4RemovalChanged), watch, UmiGtk4RemovalFree, 0);
    g_object_weak_ref(receiver, UmiGtk4RemovalReceiverGone, watch);
}
static inline void UmiGtk4CloseRemovedParentChild(GtkWidget *parent, gpointer data)
{
    (void)parent;
    gtk_window_destroy(GTK_WINDOW(data));
}
#endif
