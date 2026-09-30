/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_gtk.c
 * PURPOSE:
 *   Paper/Live connection controls. This window never offers order execution.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Paper/Live connection controls. This window never offers order execution.
 *---------------------------------------------------------------------------*/
#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "check failed: %s at %d\n", #x, __LINE__); return 1; } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, name); if (found != NULL) return found;
    }
    return NULL;
}
int main(int argc, char **argv)
{
    if (!gtk_init_check()) return 77;
    if (argc != 2) return 2;
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    g_object_ref(window);
    GtkWidget *mode = Find(GTK_WIDGET(window), "ibkr-mode");
    GtkWidget *program = Find(GTK_WIDGET(window), "ibkr-program");
    GtkWidget *port = Find(GTK_WIDGET(window), "ibkr-port");
    GtkWidget *ack = Find(GTK_WIDGET(window), "ibkr-live-ack");
    GtkWidget *connect = Find(GTK_WIDGET(window), "ibkr-connect");
    CHECK(mode && program && port && ack && connect);
    if (!strcmp(argv[1], "construction")) {
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)));
        CHECK(!gtk_widget_get_sensitive(Find(GTK_WIDGET(window), "ibkr-read")));
        CHECK(!gtk_widget_get_sensitive(Find(GTK_WIDGET(window), "ibkr-disconnect")));
    } else if (!strcmp(argv[1], "default_paper")) {
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(mode)) == 0U);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 7497);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
    } else if (!strcmp(argv[1], "mode_ports")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(mode), 1U);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 7496);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(program), 1U);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 4001);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(mode), 0U);
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(port)) == 4002);
    } else if (!strcmp(argv[1], "live_ack")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(mode), 1U);
        g_signal_emit_by_name(connect, "clicked"); /* Missing acknowledgement must not create a socket. */
        const char *text = gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(window), "ibkr-status")));
        CHECK(strstr(text, "Connection not opened") != NULL);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ack), TRUE);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(program), 1U);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(ack)));
    } else if (!strcmp(argv[1], "retained_controls")) {
        g_object_ref(connect);
        gtk_window_destroy(window);
        g_signal_emit_by_name(connect, "clicked"); /* Destroyed owner: no action. */
        g_object_unref(connect); g_object_unref(window); return 0;
    } else if (!strcmp(argv[1], "independent_windows")) {
        GtkWindow *other = UmiIbkrGtkCreate(NULL); g_object_ref(other);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(mode), 1U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(GTK_WIDGET(other), "ibkr-mode"))) == 0U);
        gtk_window_destroy(other); g_object_unref(other);
    } else if (!strcmp(argv[1], "retained_parent")) {
        GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
        GtkWidget *wrapped = UmiIbkrGtkWrap(gtk_label_new("Existing content"), parent);
        GtkWidget *button = gtk_widget_get_first_child(wrapped); g_object_ref(button);
        gtk_window_set_child(parent, wrapped);
        gtk_window_destroy(parent);
        guint before = g_list_model_get_n_items(gtk_window_get_toplevels());
        g_signal_emit_by_name(button, "clicked");
        CHECK(g_list_model_get_n_items(gtk_window_get_toplevels()) == before);
        g_object_unref(button); g_object_unref(parent);
    } else return 2;
    gtk_window_destroy(window); g_object_unref(window);
    return 0;
}
