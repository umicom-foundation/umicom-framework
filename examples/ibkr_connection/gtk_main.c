/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Paper/Live connection controls. This window never offers order execution.
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/connection_gtk4.h"
static void Activate(GtkApplication *application, gpointer data)
{
    (void)data;
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    gtk_window_set_application(window, application);
    gtk_window_present(window);
}
int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new("org.umicom.broker-connections", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), NULL);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}
