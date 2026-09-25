/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_operations/test_gtk_launcher.c
 *
 * PURPOSE:
 *   Verify additive GTK launcher ownership without touching persistent user data.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/bank_operations.h"
#include <stdio.h>

/* No click is issued while the launcher has a window root, so this lifecycle
 * test must never create a user's banking database. GTK display absence is an
 * explicit skipped result, not a successful GUI acceptance test. */
int main(void)
{
    GtkWidget *launcher, *button, *window;
    if (!gtk_init_check()) { puts("SKIP: GTK display unavailable"); return 77; }
    launcher = UmiGtk4BankOperationsLauncherCreate();
    if (launcher == NULL) return 1;
    g_object_ref_sink(launcher);
    button = gtk_widget_get_first_child(launcher);
    if (!GTK_IS_BUTTON(button)) { g_object_unref(launcher); return 1; }
    g_object_ref(button);
    g_signal_emit_by_name(button, "clicked"); /* Unparented: no database and no window. */
    window = gtk_window_new();
    g_object_ref(window); /* Keep a test-owned reference across native destruction. */
    gtk_window_set_child(GTK_WINDOW(window), launcher);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    g_object_unref(launcher);
    /* A retained button cannot call freed bar-owned state after its bar dies. */
    g_signal_emit_by_name(button, "clicked");
    g_object_unref(button);
    puts("PASS: additive banking launcher ownership"); return 0;
}
