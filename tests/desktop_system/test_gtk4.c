/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_gtk4.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/gtk4.h"
#include <string.h>
#include <stdio.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); return 1; } } while (0)
static void Dead(gpointer data, GObject *object) { (void)object; *(int *)data = 1; }
static GtkWidget *Find(GtkWidget *root)
{
    if (!strcmp(gtk_widget_get_name(root), "umicom-system-refresh")) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child); if (found) return found;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    GtkWidget *panel = UmiDesktopSystemGtk4PanelNew(); CHECK(panel); g_object_ref_sink(panel);
    GtkWidget *button = Find(panel); CHECK(button && gtk_widget_get_sensitive(button));
    if (!strcmp(argv[1], "construct")) {
        CHECK(UmiDesktopSystemGtk4Attach(panel) == UMI_STATUS_OK);
        g_object_unref(panel); return 0;
    }
    CHECK(UmiDesktopSystemGtk4Refresh(panel) == UMI_STATUS_OK);
    CHECK(UmiDesktopSystemGtk4Refresh(panel) == UMI_STATUS_BUSY);
    if (!strcmp(argv[1], "close-during-refresh")) {
        int dead = 0; g_object_weak_ref(G_OBJECT(panel), Dead, &dead); g_object_unref(panel);
        CHECK(dead); /* A worker must not hold a GTK reference. */
        for (unsigned i = 0; i < 250; ++i) { while (g_main_context_iteration(NULL, FALSE)) {} g_usleep(10000); }
        return 0;
    }
    CHECK(!strcmp(argv[1], "refresh"));
    gint64 deadline = g_get_monotonic_time() + 20000000;
    while (!gtk_widget_get_sensitive(button) && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {} g_usleep(1000);
    }
    CHECK(gtk_widget_get_sensitive(button)); g_object_unref(panel); return 0;
}
