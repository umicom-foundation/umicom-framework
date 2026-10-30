/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_window.c
 *
 * PURPOSE:
 *   Host the shared learning catalogue in a product-owned native window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/learning_catalogue.h"

/* A retained catalogue widget must stop accepting work when its window closes.
 * Unrealize also covers explicit window destruction and parent destruction;
 * close-request covers closing before the native surface is released. */
static gboolean CatalogueWindowClose(GtkWindow *window, gpointer data)
{
    (void)window;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
    return FALSE;
}
static void CatalogueWindowUnrealize(GtkWidget *window, gpointer data)
{
    (void)window;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}

/* Protect presentation across embedding callbacks and reuse live catalogues
 * through an explicit shared-window API. Preserve the original constructor
 * for review; its normal independent-window behaviour remains supported. */
#if 0
GtkWindow *UmiLearningCatalogueGtk4Present(GtkWindow *parent, UmiVersion host)
{
    if (parent == NULL || !GTK_IS_WINDOW(parent)) return NULL;
    GtkWidget *panel = UmiLearningCatalogueGtk4Create(host);
    if (panel == NULL) return NULL;
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_title(window, "Learning catalogue");
    gtk_window_set_default_size(window, 900, 720);
    gtk_window_set_transient_for(window, parent);
    gtk_window_set_destroy_with_parent(window, TRUE);
    GtkApplication *application = gtk_window_get_application(parent);
    if (application != NULL) gtk_window_set_application(window, application);
    gtk_window_set_child(window, panel);
    g_signal_connect_object(window, "close-request", G_CALLBACK(CatalogueWindowClose), panel, 0);
    g_signal_connect_object(window, "unrealize", G_CALLBACK(CatalogueWindowUnrealize), panel, 0);
    gtk_window_present(window);
    return window;
}
#endif
/* GTK owns a registry reference to live top-level windows. A client can retain
 * a destroyed GObject, so weak-reference success alone is not a live-window
 * test. Inspect this list only on the GTK owner thread. */
static int CatalogueWindowAlive(GtkWindow *window)
{
    GList *windows = gtk_window_list_toplevels();
    int alive = g_list_find(windows, window) != NULL;
    g_list_free(windows);
    return alive;
}
static int CatalogueWindowReady(GtkWindow *window, GtkWidget *panel)
{
    UmiLearningCatalogueGtkState state;
    return CatalogueWindowAlive(window) && panel != NULL &&
        UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK && !state.retired;
}
GtkWindow *UmiLearningCatalogueGtk4Present(GtkWindow *parent, UmiVersion host)
{
    if (parent == NULL || !GTK_IS_WINDOW(parent) || !CatalogueWindowAlive(parent)) return NULL;
    /* Window-added and map handlers belong to the embedding application. Keep
     * our objects alive while they run, and recheck whether they closed either
     * window before returning a borrowed live handle. */
    g_object_ref(parent);
    GtkWidget *panel = UmiLearningCatalogueGtk4Create(host);
    if (panel == NULL) { g_object_unref(parent); return NULL; }
    g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    gtk_window_set_child(window, panel);
    g_signal_connect_object(window, "close-request", G_CALLBACK(CatalogueWindowClose), panel, 0);
    g_signal_connect_object(window, "unrealize", G_CALLBACK(CatalogueWindowUnrealize), panel, 0);
    g_signal_connect_object(window, "destroy", G_CALLBACK(CatalogueWindowUnrealize), panel, 0);
    gtk_window_set_title(window, "Learning catalogue");
    gtk_window_set_default_size(window, 900, 720);
    gtk_window_set_transient_for(window, parent);
    gtk_window_set_destroy_with_parent(window, TRUE);
    GtkApplication *application = gtk_window_get_application(parent);
    /* Let GtkApplication's default window-added handler finish association and
     * install its shortcut controller before ordinary host callbacks run.
     * Calling the window-side setter first allows such a callback to destroy
     * the child while GTK still needs its application pointer. Keep that old
     * order below for review; object references alone cannot repair its state. */
#if 0
    if (application != NULL) gtk_window_set_application(window, application);
#endif
    if (application != NULL) {
        g_object_ref(application);
        gtk_application_add_window(application, window);
        g_object_unref(application);
    }
    if (CatalogueWindowAlive(parent) && CatalogueWindowReady(window, panel))
        gtk_window_present(window);
    GtkWindow *result = CatalogueWindowAlive(parent) && CatalogueWindowReady(window, panel) ? window : NULL;
    if (result == NULL) {
        UmiLearningCatalogueGtk4Retire(panel);
        if (CatalogueWindowAlive(window)) gtk_window_destroy(window);
    }
    g_object_unref(panel);
    g_object_unref(window);
    g_object_unref(parent);
    return result;
}

typedef struct CatalogueWindowSlot {
    GWeakRef window;
    UmiVersion host;
    int presenting;
} CatalogueWindowSlot;
static void CatalogueWindowSlotFree(gpointer data)
{
    CatalogueWindowSlot *slot = data;
    g_weak_ref_clear(&slot->window);
    g_free(slot);
}
GtkWindow *UmiLearningCatalogueGtk4PresentShared(GtkWindow *parent, UmiVersion host)
{
    if (parent == NULL || !GTK_IS_WINDOW(parent) || !CatalogueWindowAlive(parent)) return NULL;
    static const char key[] = "umicom-learning-catalogue-window-slot";
    g_object_ref(parent);
    CatalogueWindowSlot *slot = g_object_get_data(G_OBJECT(parent), key);
    if (slot == NULL) {
        slot = g_try_new0(CatalogueWindowSlot, 1U);
        if (slot == NULL) { g_object_unref(parent); return NULL; }
        g_weak_ref_init(&slot->window, NULL);
        g_object_set_data_full(G_OBJECT(parent), key, slot, CatalogueWindowSlotFree);
    }
    if (slot->presenting) { g_object_unref(parent); return NULL; }
    slot->presenting = 1;
    GtkWindow *window = g_weak_ref_get(&slot->window);
    GtkWidget *panel = window == NULL ? NULL : gtk_window_get_child(window);
    if (panel != NULL) g_object_ref(panel);
    GtkWindow *result = NULL;
    if (window != NULL && !CatalogueWindowReady(window, panel)) {
        g_weak_ref_set(&slot->window, NULL);
        if (CatalogueWindowAlive(window)) gtk_window_destroy(window);
        g_clear_object(&window);
        g_clear_object(&panel);
    }
    if (window != NULL) {
        /* A live shelf was admitted under a specific host version. Do not
         * silently relabel it as compatible with a different host. */
        if (slot->host.major == host.major && slot->host.minor == host.minor && slot->host.patch == host.patch) {
            gtk_window_present(window);
            if (CatalogueWindowAlive(parent) && CatalogueWindowReady(window, panel)) result = window;
        }
    } else if (CatalogueWindowAlive(parent)) {
        result = UmiLearningCatalogueGtk4Present(parent, host);
        if (result != NULL) {
            slot->host = host;
            g_weak_ref_set(&slot->window, result);
        }
    }
    slot->presenting = 0;
    g_clear_object(&panel);
    g_clear_object(&window);
    g_object_unref(parent);
    return result;
}

