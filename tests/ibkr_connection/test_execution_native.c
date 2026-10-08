/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_execution_native.c
 * PURPOSE: Exercise execution inspector controls without opening a broker connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/connection_gtk4.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name))
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, name);
        if (found != NULL)
            return found;
    }
    return NULL;
}
typedef struct Reentry
{
    GtkWindow *window;
    GtkWidget *button;
    unsigned calls;
    gboolean close;
} Reentry;
static void OnLabel(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    Reentry *probe = data;
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
    CHECK(window != NULL);
    g_object_ref(window);
    GtkWidget *review = Find(GTK_WIDGET(window), "ibkr-execution-read");
    GtkWidget *abandon = Find(GTK_WIDGET(window), "ibkr-execution-abandon");
    GtkWidget *output = Find(GTK_WIDGET(window), "ibkr-execution-output");
    CHECK(review && abandon && output);
    g_object_ref(review);
    g_object_ref(abandon);
    g_object_ref(output);
    Reentry probe = {window, review, 0U, FALSE};
    if (!strcmp(argv[1], "no-account"))
    {
        g_signal_emit_by_name(review, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "not started"));
        CHECK(
            strstr(gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(window), "ibkr-status"))), "Not connected"));
    }
    else if (!strcmp(argv[1], "abandon-empty"))
    {
        g_signal_emit_by_name(abandon, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "No execution capture"));
    }
    else if (!strcmp(argv[1], "retained"))
    {
        gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
        gtk_window_destroy(window);
        g_signal_emit_by_name(review, "clicked");
        g_signal_emit_by_name(abandon, "clicked");
        CHECK(!strcmp(before, gtk_label_get_text(GTK_LABEL(output))));
        g_free(before);
    }
    else if (!strcmp(argv[1], "reentrant") || !strcmp(argv[1], "close-notify"))
    {
        probe.close = !strcmp(argv[1], "close-notify");
        g_signal_connect(output, "notify::label", G_CALLBACK(OnLabel), &probe);
        g_signal_emit_by_name(review, "clicked");
        CHECK(probe.calls == 1U);
        g_signal_handlers_disconnect_by_data(output, &probe);
    }
    else
        return 2;
    gtk_window_destroy(window);
    g_object_unref(window);
    g_object_unref(review);
    g_object_unref(abandon);
    g_object_unref(output);
    return 0;
}
