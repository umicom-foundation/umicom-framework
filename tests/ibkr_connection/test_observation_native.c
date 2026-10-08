/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_observation_native.c
 * PURPOSE: Check native observation controls and retained-window lifetime without connecting to TWS.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                                         \
    do                                                                                                       \
    {                                                                                                        \
        if (!(value))                                                                                        \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #value);                                                   \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
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
typedef struct NotifyProbe
{
    GtkWindow *window;
    GtkWidget *button;
    unsigned calls;
    bool close;
} NotifyProbe;
static void Notify(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    NotifyProbe *probe = data;
    ++probe->calls;
    if (probe->close)
        gtk_window_destroy(probe->window);
    else if (probe->calls < 3U)
        g_signal_emit_by_name(probe->button, "clicked");
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    if (!gtk_init_check())
        return 77;
    bool pnl = !strcmp(argv[1], "pnl");
    bool depth = !strcmp(argv[1], "depth");
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window != NULL);
    g_object_ref(window);
    GtkWidget *start = Find(GTK_WIDGET(window), pnl     ? "ibkr-pnl-start"
                                                : depth ? "ibkr-depth-start"
                                                        : "ibkr-symbol-search");
    GtkWidget *stop = Find(GTK_WIDGET(window), pnl     ? "ibkr-pnl-stop"
                                               : depth ? "ibkr-depth-stop"
                                                       : "ibkr-symbol-abandon");
    GtkWidget *output = Find(GTK_WIDGET(window), pnl     ? "ibkr-pnl-output"
                                                 : depth ? "ibkr-depth-output"
                                                         : "ibkr-symbol-output");
    CHECK(start && stop && output);
    g_object_ref(start);
    g_object_ref(stop);
    g_object_ref(output);
    NotifyProbe probe = {window, start, 0U, false};
    const char *mode = argv[2];
    if (!strcmp(mode, "fields"))
    {
        CHECK(Find(GTK_WIDGET(window), pnl     ? "ibkr-pnl-contract"
                                       : depth ? "ibkr-depth-rows"
                                               : "ibkr-symbol-results"));
        CHECK(Find(GTK_WIDGET(window), pnl     ? "ibkr-pnl-model"
                                       : depth ? "ibkr-depth-smart"
                                               : "ibkr-symbol-pattern"));
        CHECK(gtk_label_get_selectable(GTK_LABEL(output)));
    }
    else if (!strcmp(mode, "unconnected"))
    {
        g_signal_emit_by_name(start, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "not started"));
        CHECK(
            strstr(gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(window), "ibkr-status"))), "Not connected"));
    }
    else if (!strcmp(mode, "stop-empty"))
    {
        g_signal_emit_by_name(stop, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)),
                     (pnl || depth) ? "not queued" : "No instrument search"));
    }
    else if (!strcmp(mode, "retained"))
    {
        gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
        gtk_window_destroy(window);
        g_signal_emit_by_name(start, "clicked");
        g_signal_emit_by_name(stop, "clicked");
        CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
        g_free(before);
    }
    else if (!strcmp(mode, "reentrant") || !strcmp(mode, "close-notify"))
    {
        probe.close = !strcmp(mode, "close-notify");
        g_signal_connect(output, "notify::label", G_CALLBACK(Notify), &probe);
        g_signal_emit_by_name(start, "clicked");
        CHECK(probe.calls == 1U);
        g_signal_handlers_disconnect_by_data(output, &probe);
    }
    else if (!strcmp(mode, "apply-empty") && !pnl)
    {
        GtkWidget *apply = Find(GTK_WIDGET(window), "ibkr-symbol-apply");
        GtkWidget *contract = Find(GTK_WIDGET(window), "ibkr-quote-contract");
        CHECK(apply && contract);
        gtk_editable_set_text(GTK_EDITABLE(contract), "123");
        g_signal_emit_by_name(apply, "clicked");
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "123"));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "completed current result"));
    }
    else
        return 2;
    gtk_window_destroy(window);
    g_object_unref(window);
    g_object_unref(start);
    g_object_unref(stop);
    g_object_unref(output);
    return 0;
}
