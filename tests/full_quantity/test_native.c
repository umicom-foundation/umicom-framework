/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/full_quantity/test_native.c
 * PURPOSE: Exercise the actual full-quantity controls without opening a broker connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
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
/* Broker controls inside collapsed panels still belong to the window. Framework logical lookup replaces rendered-child recursion so fixtures inspect those controls without expanding panels or starting a broker action. The previous implementation is retained for engineering review. */
#if 0
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
#endif
static GtkWidget *Find(GtkWidget *root, const char *name)
{
    return umi_gtk4_automation_find_named_widget(root, name);
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
    g_object_ref(window);
    GtkWidget *review = Find(GTK_WIDGET(window), "ibkr-fill-review");
    GtkWidget *start = Find(GTK_WIDGET(window), "ibkr-fill-start");
    GtkWidget *price = Find(GTK_WIDGET(window), "ibkr-fill-price");
    GtkWidget *mode = Find(GTK_WIDGET(window), "ibkr-fill-mode");
    GtkWidget *output = Find(GTK_WIDGET(window), "ibkr-fill-output");
    CHECK(review && start && price && mode && output);
    gtk_editable_set_text(GTK_EDITABLE(price), "4.25");
    g_object_ref(review);
    g_object_ref(start);
    g_object_ref(output);
    Reentry probe = {window, review, 0U, FALSE};
    if (!strcmp(argv[1], "fields"))
    {
        g_signal_emit_by_name(review, "clicked");
        const char *text = gtk_label_get_text(GTK_LABEL(output));
        CHECK(strstr(text, "tif=FOK") && strstr(text, "UNCONFIRMED") && strstr(text, "No order was sent"));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(mode), 1U);
        g_signal_emit_by_name(review, "clicked");
        text = gtk_label_get_text(GTK_LABEL(output));
        CHECK(strstr(text, "allOrNone=true") && strstr(text, "tif=DAY") && strstr(text, "minQty=unset"));
    }
    else if (!strcmp(argv[1], "no-quote"))
    {
        g_signal_emit_by_name(start, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "not started"));
        CHECK(
            strstr(gtk_label_get_text(GTK_LABEL(Find(GTK_WIDGET(window), "ibkr-status"))), "Not connected"));
    }
    else if (!strcmp(argv[1], "invalid"))
    {
        gtk_editable_set_text(GTK_EDITABLE(price), "4,25");
        g_signal_emit_by_name(review, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "Enter positive"));
    }
    else if (!strcmp(argv[1], "retained"))
    {
        gchar *before = g_strdup(gtk_label_get_text(GTK_LABEL(output)));
        gtk_window_destroy(window);
        g_signal_emit_by_name(review, "clicked");
        g_signal_emit_by_name(start, "clicked");
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
    g_object_unref(start);
    g_object_unref(output);
    return 0;
}
