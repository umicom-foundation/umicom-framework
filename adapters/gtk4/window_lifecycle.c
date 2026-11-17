/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/window_lifecycle.c
 *
 * PURPOSE:
 *   Reuse native removal rules and retain owned windows across presentation observers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Applications keep their runtime ownership token; Framework owns the common
 * GTK window boundary. Native removal can precede finalisation, so strong
 * references alone do not establish that either window is still usable. */
UmiStatus UmiGtk4PresentOwnedWindow(GtkWindow *owner, GtkWindow *child)
{
    static const char guard_key[] = "umicom-owned-window-present-active";
    if (!GTK_IS_WINDOW(owner) || !GTK_IS_WINDOW(child) || owner == child)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4WindowIsOpen(owner) || !UmiGtk4WindowIsOpen(child))
        return UMI_STATUS_CANCELLED;
    if (gtk_window_get_transient_for(child) != owner)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(child), guard_key) != NULL)
        return UMI_STATUS_BUSY;

    g_object_ref(owner);
    g_object_ref(child);
    g_object_set_data(G_OBJECT(child), guard_key, GINT_TO_POINTER(1));
/* Visibility observers can close or reparent the native child synchronously. Validate the owner and child after showing, before GTK attempts native focus; retain the previous presentation call for review. The former implementation is retained for owner review. */
#if 0
    gtk_window_present(child);
#endif
    gtk_widget_set_visible(GTK_WIDGET(child), TRUE);
    if (UmiGtk4WindowIsOpen(owner) && UmiGtk4WindowIsOpen(child) &&
        gtk_window_get_transient_for(child) == owner &&
        gtk_widget_get_visible(GTK_WIDGET(child)))
        gtk_window_present(child);
    UmiStatus status = UmiGtk4WindowIsOpen(owner) &&
        UmiGtk4WindowIsOpen(child) &&
        gtk_window_get_transient_for(child) == owner &&
        gtk_widget_get_visible(GTK_WIDGET(child))
        ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    g_object_set_data(G_OBJECT(child), guard_key, NULL);
    g_object_unref(child);
    g_object_unref(owner);
    return status;
}

/* Hide and present share the child transition guard. This prevents a visibility
 * observer from starting a competing transition while a caller prepares to
 * dispatch a command; application runtime tokens remain application-owned. */
UmiStatus UmiGtk4HideOwnedWindow(GtkWindow *owner, GtkWindow *child)
{
    static const char guard_key[] = "umicom-owned-window-present-active";
    if (!GTK_IS_WINDOW(owner) || !GTK_IS_WINDOW(child) || owner == child)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4WindowIsOpen(owner) || !UmiGtk4WindowIsOpen(child))
        return UMI_STATUS_CANCELLED;
    if (gtk_window_get_transient_for(child) != owner)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(child), guard_key) != NULL)
        return UMI_STATUS_BUSY;
    g_object_ref(owner);
    g_object_ref(child);
    g_object_set_data(G_OBJECT(child), guard_key, GINT_TO_POINTER(1));
    gtk_widget_set_visible(GTK_WIDGET(child), FALSE);
    UmiStatus status = UmiGtk4WindowIsOpen(owner) &&
        UmiGtk4WindowIsOpen(child) &&
        gtk_window_get_transient_for(child) == owner &&
        !gtk_widget_get_visible(GTK_WIDGET(child))
        ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    g_object_set_data(G_OBJECT(child), guard_key, NULL);
    g_object_unref(child);
    g_object_unref(owner);
    return status;
}

/* A notebook index is only a position. Resolve the page before calling GTK,
 * retain its identity through observers, and verify that identity afterward.
 * Applications can reuse this boundary without duplicating GTK lifetime rules. */
static GtkWidget *OwnedNotebookPage(GtkNotebook *notebook, GtkWidget *target)
{
    int count = gtk_notebook_get_n_pages(notebook);
    for (int index = 0; index < count; ++index) {
        GtkWidget *page = gtk_notebook_get_nth_page(notebook, index);
        if (page == target || (page != NULL && gtk_widget_is_ancestor(target, page)))
            return page;
    }
    return NULL;
}

UmiStatus UmiGtk4NotebookSelectOwnedTarget(GtkWindow *owner,
    GtkNotebook *notebook, GtkWidget *target)
{
    static const char guard_key[] = "umicom-owned-notebook-selection-active";
    if (!GTK_IS_WINDOW(owner) || !GTK_IS_NOTEBOOK(notebook) || !GTK_IS_WIDGET(target))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4WindowIsOpen(owner)) return UMI_STATUS_CANCELLED;
    if (g_object_get_data(G_OBJECT(notebook), guard_key) != NULL)
        return UMI_STATUS_BUSY;
    if (gtk_widget_get_root(GTK_WIDGET(notebook)) != GTK_ROOT(owner) ||
        gtk_widget_get_root(target) != GTK_ROOT(owner))
        return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *page = OwnedNotebookPage(notebook, target);
    if (page == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!gtk_widget_get_visible(page)) return UMI_STATUS_UNAVAILABLE;

    g_object_ref(owner);
    g_object_ref(notebook);
    g_object_ref(target);
    g_object_ref(page);
    g_object_set_data(G_OBJECT(notebook), guard_key, GINT_TO_POINTER(1));
    gtk_notebook_set_current_page(notebook, gtk_notebook_page_num(notebook, page));
    /* Never pass -1 to get_nth_page here: GTK defines it as the last page,
     * which could incorrectly validate a notebook with no current selection. */
    int current = gtk_notebook_get_current_page(notebook);
    UmiStatus status = UmiGtk4WindowIsOpen(owner) &&
        gtk_widget_get_root(GTK_WIDGET(notebook)) == GTK_ROOT(owner) &&
        gtk_widget_get_root(target) == GTK_ROOT(owner) &&
        OwnedNotebookPage(notebook, target) == page &&
        gtk_widget_get_visible(page) && current >= 0 &&
        gtk_notebook_get_nth_page(notebook, current) == page
        ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    g_object_set_data(G_OBJECT(notebook), guard_key, NULL);
    g_object_unref(page);
    g_object_unref(target);
    g_object_unref(notebook);
    g_object_unref(owner);
    return status;
}
