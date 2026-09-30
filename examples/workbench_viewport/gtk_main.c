/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/workbench_viewport/gtk_main.c
 * PURPOSE:
 *   Thin host: all widgets and layout services belong to Framework.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Thin host: all widgets and layout services belong to Framework. */
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include "umicom/workbench_layout/viewport_gtk4.h"

static void Activate(GtkApplication *application, gpointer context)
{
    (void)context;
    GtkWindow *window = UmiWorkbenchViewportGallery(application);
    if (window) gtk_window_present(window);
}

static int Run(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new(
        "org.umicom.ComponentWorkbench", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), NULL);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return result;
}

#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR command, int show)
{
    (void)instance;
    (void)previous;
    (void)command;
    (void)show;
    return Run(0, NULL);
}
#else
int main(int argc, char **argv) { return Run(argc, argv); }
#endif
