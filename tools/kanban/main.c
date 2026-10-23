/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/kanban/main.c
 * PURPOSE: Launch the shared native planning window without duplicating board or storage rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_window.h"
static void Activate(GtkApplication *application, gpointer data)
{
    (void)data;
    GtkWindow *window = gtk_application_get_active_window(application);
    if (window == NULL) window = UmiKanbanGtk4CreateWindow(application);
    if (window != NULL) gtk_window_present(window);
}
int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new("org.umicom.Kanban", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), NULL);
    int status = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application); return status;
}
