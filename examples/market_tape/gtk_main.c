/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/market_tape/gtk_main.c
 * PURPOSE:
 *   Present the shared practice workspace without creating an application-local model.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: examples/market_tape/gtk_main.c
 * Present the shared practice workspace without creating an application-local model.
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/market_tape.h"
static void Activate(GtkApplication *application, gpointer unused)
{
    (void)unused;
    GtkWindow *window = UmiMarketTapeGtkCreate(NULL);
    if (window == NULL) { g_application_quit(G_APPLICATION(application)); return; }
    gtk_window_set_application(window, application);
    gtk_window_present(window);
}
int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new("org.umicom.market-tape.practice", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), NULL);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}
