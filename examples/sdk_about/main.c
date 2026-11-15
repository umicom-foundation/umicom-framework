/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/sdk_about/main.c
 *
 * PURPOSE:
 *   Demonstrate an independent C application using the installed native About service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/about_dialog.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
typedef struct ExampleState { UmiStatus status; gboolean opening; } ExampleState;
static void Activate(GtkApplication *application, gpointer data)
{
    ExampleState *state = data;
    if (state->opening) return;
    state->opening = TRUE;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing != NULL && UmiGtk4WindowIsOpen(existing)) {
        gtk_window_present(existing);
        state->opening = FALSE;
        return;
    }
    const UmiGtk4AboutDialogConfig config = {
        {"Umicom SDK Example", "Umicom Foundation", "umicom-sdk-about"},
        "1.0.0", "An independent C application using an installed Framework package.",
        GTK_LICENSE_MIT_X11
    };
    GtkWindow *window = NULL;
    state->status = UmiGtk4AboutDialogCreate(NULL, &config, &window);
    if (state->status == UMI_STATUS_OK) {
        /* GtkApplication now governs application lifetime. Framework's owned
         * creation reference remains ours until the end of this callback. */
        gtk_application_add_window(application, window);
        if (UmiGtk4WindowIsOpen(window)) gtk_window_present(window);
        if (!UmiGtk4WindowIsOpen(window) || !gtk_widget_get_visible(GTK_WIDGET(window)))
            state->status = UMI_STATUS_CANCELLED;
        if (state->status != UMI_STATUS_OK && UmiGtk4WindowIsOpen(window))
            gtk_window_destroy(window);
        g_object_unref(window);
    }
    state->opening = FALSE;
    if (state->status != UMI_STATUS_OK) {
        fprintf(stderr, "About window creation/presentation returned status %d.\n", (int)state->status);
        g_application_quit(G_APPLICATION(application));
    }
}
int main(int argc, char **argv)
{
    ExampleState state = {UMI_STATUS_OK, FALSE};
    GtkApplication *application = gtk_application_new("org.umicom.sdk.about", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), &state);
    int exit_code = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return state.status == UMI_STATUS_OK ? exit_code : 1;
}
