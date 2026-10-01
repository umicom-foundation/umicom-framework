/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/window_lifecycle.c
 * PURPOSE: Expose the proven removal watch without duplicating native lifetime rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/window_lifecycle.h"
#include "window_removal_private.h"

/* Object finalisation is later than native removal when windows are retained.
 * Reuse the existing observer already used by banking and broker windows. */
gboolean UmiGtk4WindowIsOpen(GtkWindow *window)
{
    return GTK_IS_WINDOW(window) && UmiGtk4WindowRegistered(window);
}

/* Keep the established weak-receiver and reentrant-callback implementation in
 * one place. The private helpers remain intact for existing adapter callers. */
UmiStatus UmiGtk4WatchWindowClosed(GtkWindow *window, GObject *receiver,
    UmiGtk4WindowClosedCallback callback, gpointer data)
{
    if (!GTK_IS_WINDOW(window) || !G_IS_OBJECT(receiver) || callback == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Immediate callbacks get the same live-object guarantee as removal
     * callbacks, even if application code releases its original references. */
    g_object_ref(window);
    g_object_ref(receiver);
    UmiGtk4ObserveWindowRemoval(window, receiver, callback, data);
    g_object_unref(receiver);
    g_object_unref(window);
    return UMI_STATUS_OK;
}
