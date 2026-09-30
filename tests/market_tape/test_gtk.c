/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/market_tape/test_gtk.c
 * PURPOSE:
 *   Real GTK object lifecycle checks. Exit 77 means no usable display, not pass.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Foundation | Sammy Hegab | MIT
 * Real GTK object lifecycle checks. Exit 77 means no usable display, not pass. */
#include "umicom/ui/gtk4/market_tape.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
static void Click(GtkWidget *button) { g_signal_emit_by_name(button, "clicked"); }
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    GtkWindow *first = UmiMarketTapeGtkCreate(NULL);
    CHECK(first != NULL);
    g_object_ref(first);
    GtkWidget *next = Find(GTK_WIDGET(first), "Next observation");
    CHECK(next != NULL);
    if (strcmp(argv[1], "actions") == 0) {
        Click(next);
        GtkWidget *alpha = Find(GTK_WIDGET(first), "Select ALPHA");
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(alpha)), "100.000") != NULL);
        Click(Find(GTK_WIDGET(first), "Advance 3 seconds"));
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(alpha)), "NOT current") != NULL);
    } else if (strcmp(argv[1], "gap_reset") == 0) {
        Click(next); Click(Find(GTK_WIDGET(first), "Introduce gap"));
        CHECK(!gtk_widget_get_sensitive(next));
        Click(Find(GTK_WIDGET(first), "New epoch"));
        CHECK(gtk_widget_get_sensitive(next));
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(Find(GTK_WIDGET(first), "Select ALPHA"))), "no quote") != NULL);
    } else if (strcmp(argv[1], "independence") == 0) {
        GtkWindow *second = UmiMarketTapeGtkCreate(NULL); CHECK(second != NULL);
        Click(next);
        CHECK(strstr(gtk_button_get_label(GTK_BUTTON(Find(GTK_WIDGET(second), "Select ALPHA"))), "no quote") != NULL);
        gtk_window_destroy(second);
    } else if (strcmp(argv[1], "retained_child") == 0) {
        g_object_ref(next);
        gtk_window_destroy(first);
        Click(next); /* The owner is retained but already closed. */
        g_object_unref(first); first = NULL;
        Click(next); /* The owner is now gone; object-bound signal is disconnected. */
        g_object_unref(next);
    } else if (strncmp(argv[1], "wrapper_", 8U) == 0) {
        GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
        GtkWidget *content = gtk_label_new("Existing Trader workspace");
        GtkWidget *wrapper = UmiMarketTapeGtkWrap(content, parent);
        CHECK(wrapper != content && gtk_widget_get_parent(content) == wrapper);
        CHECK(UmiMarketTapeGtkWrap(content, parent) == content);
        gtk_window_set_child(parent, wrapper);
        GtkWidget *entry = gtk_widget_get_first_child(wrapper); g_object_ref(entry);
        if (strcmp(argv[1], "wrapper_parent") == 0) {
            gtk_window_destroy(parent); CHECK(!gtk_widget_get_sensitive(entry)); Click(entry);
        } else {
            gtk_window_set_child(parent, NULL); Click(entry); /* Unrooted retained child. */
            gtk_window_destroy(parent);
        }
        g_object_unref(parent); Click(entry); g_object_unref(entry);
    } else CHECK(strcmp(argv[1], "construction") == 0);
    if (first != NULL) { gtk_window_destroy(first); g_object_unref(first); }
    while (g_main_context_iteration(NULL, FALSE)) { }
    return 0;
}
