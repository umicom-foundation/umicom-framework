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
