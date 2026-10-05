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
typedef struct ReportReentry {
    GtkWidget *button;
    unsigned calls;
} ReportReentry;
/* Reenter from status publication to verify that a failed export cannot start
 * another operation while the first callback still owns its window state. */
static void ReenterReport(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    ReportReentry *probe = context;
    ++probe->calls;
    if (probe->calls < 3U) g_signal_emit_by_name(probe->button, "clicked");
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
    /* Position controls cannot invent account data or quote identities. Retained
     * controls also remain inert after the owning connection window is closed. */
    if (strncmp(argv[1], "position-", 9U) == 0) {
        GtkWidget *capture = Find(GTK_WIDGET(window), "ibkr-position-capture");
        GtkWidget *choose = Find(GTK_WIDGET(window), "ibkr-position-quote");
        GtkWidget *detail = Find(GTK_WIDGET(window), "ibkr-position-detail");
        GtkWidget *contract = Find(GTK_WIDGET(window), "ibkr-quote-contract");
        CHECK(capture && choose && detail && contract);
        g_object_ref(capture); g_object_ref(choose); g_object_ref(contract);
        gtk_editable_set_text(GTK_EDITABLE(contract), "456");
        if (!strcmp(argv[1], "position-retained")) gtk_window_destroy(window);
        g_signal_emit_by_name(capture, "clicked");
        g_signal_emit_by_name(choose, "clicked");
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "456"));
        if (!strcmp(argv[1], "position-no-capture"))
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "not changed") != NULL);
        g_object_unref(capture); g_object_unref(choose); g_object_unref(contract);
        gtk_window_destroy(window); g_object_unref(window); return 0;
    }
    if (strncmp(argv[1], "report-", 7U) == 0) {
        GtkWidget *button = Find(GTK_WIDGET(window), "ibkr-export-report");
        GtkWidget *path = Find(GTK_WIDGET(window), "ibkr-export-path");
        GtkWidget *notice = Find(GTK_WIDGET(window), "ibkr-export-status");
        CHECK(button != NULL && path != NULL && notice != NULL);
        gchar *identity = g_uuid_string_random();
        gchar *leaf = g_strdup_printf("umicom-report-%s.csv", identity);
        gchar *destination = g_build_filename(g_get_tmp_dir(), leaf, NULL);
        g_free(leaf); g_free(identity);
        CHECK(!g_file_test(destination, G_FILE_TEST_EXISTS));
        gtk_editable_set_text(GTK_EDITABLE(path), destination);
        ReportReentry probe = {button, 0U};
        g_object_ref(button); g_object_ref(notice);
        if (strcmp(argv[1], "report-reentrant") == 0)
            g_signal_connect(notice, "notify::label", G_CALLBACK(ReenterReport), &probe);
        if (strcmp(argv[1], "report-retained") == 0) gtk_window_destroy(window);
        g_signal_emit_by_name(button, "clicked");
        CHECK(!g_file_test(destination, G_FILE_TEST_EXISTS));
        if (strcmp(argv[1], "report-reentrant") == 0) CHECK(probe.calls == 1U);
        if (strcmp(argv[1], "report-retained") != 0)
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(notice)), "Report not written") != NULL);
        g_signal_handlers_disconnect_by_data(notice, &probe);
        g_object_unref(notice); g_object_unref(button); g_free(destination);
        gtk_window_destroy(window); g_object_unref(window);
        return 0;
    }
    if (!strcmp(argv[1], "construction")) {
        CHECK(!gtk_widget_get_visible(GTK_WIDGET(window)));
        CHECK(!gtk_widget_get_sensitive(Find(GTK_WIDGET(window), "ibkr-read")));
        CHECK(!gtk_widget_get_sensitive(Find(GTK_WIDGET(window), "ibkr-disconnect")));
    } else if (!strcmp(argv[1], "quote_controls")) {
        GtkWidget *start = Find(GTK_WIDGET(window), "ibkr-quote-start");
        GtkWidget *stop = Find(GTK_WIDGET(window), "ibkr-quote-stop");
        CHECK(GTK_IS_ENTRY(Find(GTK_WIDGET(window), "ibkr-quote-contract")));
        CHECK(GTK_IS_ENTRY(Find(GTK_WIDGET(window), "ibkr-quote-exchange")));
        CHECK(GTK_IS_LABEL(Find(GTK_WIDGET(window), "ibkr-quote-output")));
        CHECK(GTK_IS_BUTTON(start) && GTK_IS_BUTTON(stop));
        CHECK(!gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
        /* Retained controls lose authority with the native window. Emitting
         * signals after close must not revive a socket or subscription. */
        g_object_ref(start); g_object_ref(stop);
        gtk_window_destroy(window);
        g_signal_emit_by_name(start, "clicked");
        g_signal_emit_by_name(stop, "clicked");
        g_object_unref(start); g_object_unref(stop); g_object_unref(window);
        return 0;
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
