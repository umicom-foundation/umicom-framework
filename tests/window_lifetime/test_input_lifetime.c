/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/window_lifetime/test_input_lifetime.c
 *
 * PURPOSE:
 *   Verify input focus is released while a closing native window still has a surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/window_input_lifetime_private.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

typedef struct InputLifetimeObservation {
    unsigned calls;
    gboolean had_surface;
    gboolean focus_cleared;
} InputLifetimeObservation;

static void ObserveUnrealize(GtkWidget *widget, gpointer data)
{
    InputLifetimeObservation *observation = data;
    ++observation->calls;
    observation->had_surface = gtk_widget_get_realized(widget);
    observation->focus_cleared = gtk_window_get_focus(GTK_WINDOW(widget)) == NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2 || (strcmp(argv[1], "direct") && strcmp(argv[1], "parent")))
        return 2;
    if (!gtk_init_check()) return 77;
    fprintf(stderr, "GTK %u.%u.%u; window input lifetime: %s\n",
        gtk_get_major_version(), gtk_get_minor_version(), gtk_get_micro_version(), argv[1]);
    int failed = 0;
    GtkWindow *parent = NULL;
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    GtkWidget *entry = gtk_entry_new();
    gtk_window_set_child(window, entry);
    g_object_ref(entry);
    gtk_editable_set_text(GTK_EDITABLE(entry), "retained input");
    UmiGtk4PrepareWindowInputLifetime(window);
    UmiGtk4PrepareWindowInputLifetime(window);
    InputLifetimeObservation observation = {0};
    g_signal_connect(window, "unrealize", G_CALLBACK(ObserveUnrealize), &observation);
    if (!strcmp(argv[1], "parent")) {
        parent = GTK_WINDOW(gtk_window_new());
        g_object_ref(parent);
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        gtk_window_present(parent);
    }
    gtk_window_present(window);
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (!gtk_widget_get_mapped(GTK_WIDGET(window)) && g_get_monotonic_time() < deadline) {
        for (unsigned step = 0; step < 32U && g_main_context_pending(NULL); ++step)
            (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    CHECK(gtk_widget_get_realized(GTK_WIDGET(window)));
    CHECK(gtk_widget_get_mapped(GTK_WIDGET(window)));
    gtk_window_set_focus(window, entry);
    CHECK(gtk_window_get_focus(window) != NULL);
    /* Destroy-with-parent observes parent disposal. Keeping our extra parent
     * reference prevented that signal; retain the old fixture for review.
     * The child remains owned while its retired input is inspected. */
#if 0
    if (parent != NULL) gtk_window_destroy(parent);
    else gtk_window_destroy(window);
#endif
    if (parent != NULL) {
        gtk_window_destroy(parent);
        g_clear_object(&parent);
    } else gtk_window_destroy(window);
    fprintf(stderr, "unrealize calls=%u surface=%d focus-cleared=%d\n",
        observation.calls, observation.had_surface, observation.focus_cleared);
    CHECK(observation.calls == 1U && observation.had_surface && observation.focus_cleared);
    CHECK(!gtk_widget_get_realized(GTK_WIDGET(window)));
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "retained input") == 0);
    /* Deliver any backend messages still queued after native teardown. A stale
     * input callback must not outlive its retired window. */
    for (unsigned step = 0; step < 64U && g_main_context_pending(NULL); ++step)
        (void)g_main_context_iteration(NULL, FALSE);
cleanup:
    gtk_window_destroy(window);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    g_object_unref(entry);
    g_object_unref(window);
    return failed;
}
