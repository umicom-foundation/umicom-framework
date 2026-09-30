/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_window_removal.c
 * PURPOSE: Check logical GTK window closure, callback cancellation and reentrant removal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/window_removal_private.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); return 1; } } while (0)
typedef struct Probe {
    unsigned calls;
    GtkWindow *other;
    GObject **release;
} Probe;
static void Removed(GtkWidget *window, gpointer data)
{
    (void)window;
    Probe *probe = data;
    ++probe->calls;
    if (probe->other != NULL) gtk_window_destroy(probe->other);
    if (probe->release != NULL) g_clear_object(probe->release);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    GtkWindow *first = GTK_WINDOW(gtk_window_new()); g_object_ref(first);
    GtkWindow *second = GTK_WINDOW(gtk_window_new()); g_object_ref(second);
    GObject *receiver = g_object_new(G_TYPE_OBJECT, NULL);
    Probe a = {0}, b = {0};
    UmiGtk4ObserveWindowRemoval(first, receiver, Removed, &a);
    if (strcmp(argv[1], "retained") == 0) {
        gtk_window_destroy(first);
        CHECK(a.calls == 1U && !UmiGtk4WindowRegistered(first));
        gtk_window_destroy(first); CHECK(a.calls == 1U);
    } else if (strcmp(argv[1], "unrelated") == 0) {
        gtk_window_destroy(second); CHECK(a.calls == 0U);
        gtk_window_destroy(first); CHECK(a.calls == 1U);
    } else if (strcmp(argv[1], "receiver-first") == 0) {
        g_clear_object(&receiver);
        gtk_window_destroy(first); CHECK(a.calls == 0U);
    } else if (strcmp(argv[1], "reentrant") == 0) {
        UmiGtk4ObserveWindowRemoval(second, G_OBJECT(second), Removed, &b);
        a.other = second;
        gtk_window_destroy(first);
        CHECK(a.calls == 1U && b.calls == 1U);
    } else if (strcmp(argv[1], "release-receiver") == 0) {
        a.release = &receiver;
        gtk_window_destroy(first);
        CHECK(a.calls == 1U && receiver == NULL);
    } else return 2;
    gtk_window_destroy(first); gtk_window_destroy(second);
    g_object_unref(first); g_object_unref(second); g_clear_object(&receiver);
    return 0;
}
