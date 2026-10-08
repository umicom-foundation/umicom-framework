/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_catalog_native.c
 * PURPOSE: Check discovery controls remain inert after closure and reject nested actions.
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
    const char *cases[] = {"fields",       "unconnected",    "retained",    "reentrant",     "close-notify",
                           "search-empty", "search-missing", "search-wrap", "search-unicode"};
    bool known = false;
    for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
        if (!strcmp(argv[1], cases[i]))
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window);
    g_object_ref(window);
    GtkWidget *button = Find(GTK_WIDGET(window), "ibkr-catalog-read"),
              *output = Find(GTK_WIDGET(window), "ibkr-catalog-output");
    CHECK(button && output);
    g_object_ref(button);
    g_object_ref(output);
    Probe probe = {window, button, 0U, false};
    if (!strcmp(argv[1], "fields"))
    {
        CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(Find(GTK_WIDGET(window), "ibkr-catalog-text"))));
        CHECK(Find(GTK_WIDGET(window), "ibkr-catalog-search") &&
              Find(GTK_WIDGET(window), "ibkr-catalog-find"));
    }
    else if (!strncmp(argv[1], "search-", 7U))
    {
        GtkWidget *find = Find(GTK_WIDGET(window), "ibkr-catalog-find");
        GtkWidget *search = Find(GTK_WIDGET(window), "ibkr-catalog-search");
        GtkWidget *view = Find(GTK_WIDGET(window), "ibkr-catalog-text");
        CHECK(find && search && view);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        gtk_text_buffer_set_text(buffer, "GAIN gain caf\xc3\xa9", -1);
        const char *needle = !strcmp(argv[1], "search-empty")     ? ""
                             : !strcmp(argv[1], "search-missing") ? "absent"
                             : !strcmp(argv[1], "search-unicode") ? "caf\xc3\xa9"
                                                                  : "gain";
        gtk_editable_set_text(GTK_EDITABLE(search), needle);
        g_signal_emit_by_name(find, "clicked");
        GtkTextIter start, end;
        if (!strcmp(argv[1], "search-empty") || !strcmp(argv[1], "search-missing"))
        {
            CHECK(!gtk_text_buffer_get_selection_bounds(buffer, &start, &end));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), !*needle ? "Enter a scan" : "No matching"));
        }
        else
        {
            CHECK(gtk_text_buffer_get_selection_bounds(buffer, &start, &end));
            CHECK(gtk_text_iter_get_offset(&start) == (!strcmp(argv[1], "search-unicode") ? 10 : 0));
            if (!strcmp(argv[1], "search-wrap"))
            {
                g_signal_emit_by_name(find, "clicked");
                CHECK(gtk_text_buffer_get_selection_bounds(buffer, &start, &end));
                CHECK(gtk_text_iter_get_offset(&start) == 5);
                g_signal_emit_by_name(find, "clicked");
                CHECK(gtk_text_buffer_get_selection_bounds(buffer, &start, &end));
                CHECK(gtk_text_iter_get_offset(&start) == 0);
            }
        }
    }
    else if (!strcmp(argv[1], "unconnected"))
    {
        g_signal_emit_by_name(button, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(output)), "not requested"));
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
