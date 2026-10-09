/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_completed_export_native.c
 * PURPOSE: Verify completed-report controls cannot write or access a logically closed window.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(v)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(v))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
/* Broker controls inside collapsed panels still belong to the window. Framework logical lookup replaces rendered-child recursion so fixtures inspect those controls without expanding panels or starting a broker action. The previous implementation is retained for engineering review. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name))
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, name);
        if (found)
            return found;
    }
    return NULL;
}
#endif
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    return umi_gtk4_automation_find_named_widget(root, name);
}
typedef struct Probe
{
    GtkWindow *window;
    GtkWidget *button;
    unsigned calls;
    bool close;
} Probe;
static void Notify(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    Probe *probe = data;
    ++probe->calls;
    if (probe->close)
        gtk_window_destroy(probe->window);
    else if (probe->calls < 3U)
        g_signal_emit_by_name(probe->button, "clicked");
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check())
        return 77;
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window);
    g_object_ref(window);
    GtkWidget *button = Find(GTK_WIDGET(window), "ibkr-completed-export-report");
    GtkWidget *all = Find(GTK_WIDGET(window), "ibkr-completed-export-path");
    GtkWidget *output = Find(GTK_WIDGET(window), "ibkr-completed-export-status");
    CHECK(button && all && output);
    g_object_ref(button);
    g_object_ref(output);
    Probe probe = {window, button, 0U, false};
    gchar *unusedPath = g_build_filename(g_get_tmp_dir(), "umicom-unconnected-completed-report.csv", NULL);
    /* There is no broker connection or accepted capture, so even a valid path
     * must fail before any file worker is started. */
    gtk_editable_set_text(GTK_EDITABLE(all), !strcmp(argv[1], "relative-path") ? "report.csv" : unusedPath);
    g_free(unusedPath);
    if (!strcmp(argv[1], "fields"))
    {
        CHECK(GTK_IS_ENTRY(all));
        CHECK(gtk_entry_get_max_length(GTK_ENTRY(all)) > 0);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "No completed-order report"));
    }
    else if (!strcmp(argv[1], "unconnected") || !strcmp(argv[1], "relative-path"))
    {
        g_signal_emit_by_name(button, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "Report not written"));
    }
    else if (!strcmp(argv[1], "retained"))
    {
        gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
        gtk_window_destroy(window);
        g_signal_emit_by_name(button, "clicked");
        CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
        g_free(before);
    }
    else if (!strcmp(argv[1], "reentrant") || !strcmp(argv[1], "close-notify"))
    {
        probe.close = !strcmp(argv[1], "close-notify");
        g_signal_connect(output, "notify::label", G_CALLBACK(Notify), &probe);
        g_signal_emit_by_name(button, "clicked");
        CHECK(probe.calls == 1U);
        g_signal_handlers_disconnect_by_data(output, &probe);
    }
    else
        return 2;
    gtk_window_destroy(window);
    g_object_unref(window);
    g_object_unref(button);
    g_object_unref(output);
    return 0;
}
