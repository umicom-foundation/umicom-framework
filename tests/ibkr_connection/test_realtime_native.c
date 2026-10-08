/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_native.c
 * PURPOSE: Check streaming controls remain inert after closure and guard reentrant publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
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
    GtkWidget *button = Find(GTK_WIDGET(window), "ibkr-stream-start");
    GtkWidget *all = Find(GTK_WIDGET(window), "ibkr-stream-rth");
    GtkWidget *output = Find(GTK_WIDGET(window), "ibkr-stream-output");
    CHECK(button && all && output);
    GtkWidget *chart = Find(GTK_WIDGET(window), "ibkr-stream-chart");
    GtkWidget *visible = Find(GTK_WIDGET(window), "ibkr-stream-visible");
    GtkWidget *offset = Find(GTK_WIDGET(window), "ibkr-stream-offset");
    CHECK(chart && visible && offset);
    g_object_ref(chart);
    g_object_ref(button);
    g_object_ref(output);
    Probe probe = {window, button, 0U, false};
    if (!strcmp(argv[1], "fields"))
    {
        CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(all)));
        CHECK(gtk_label_get_selectable(GTK_LABEL(output)));
        CHECK(GTK_IS_DRAWING_AREA(chart));
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(visible)) == 128);
    }
    else if (!strcmp(argv[1], "unconnected"))
    {
        g_signal_emit_by_name(button, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "not started"));
    }
    else if (!strcmp(argv[1], "retained"))
    {
        gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
        gtk_window_destroy(window);
        g_signal_emit_by_name(button, "clicked");
        CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
        g_free(before);
    }
    else if (!strcmp(argv[1], "range-empty"))
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 64);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(offset), 16);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "No streaming subscription"));
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
    g_object_unref(chart);
    return 0;
}
