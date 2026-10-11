/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_caption_files_gtk4.c
 * PURPOSE: Exercise local caption file workers and refusal to replace a changed draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/captions.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(x))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                        \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
static const char input[] = "1\n00:00:01,000 --> 00:00:02,000\nImported caption\n\n";
static const char draft[] = "1\n00:00:03,000 --> 00:00:04,000\nMy draft\n\n";
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag && !strcmp(tag, id))
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found)
            return found;
    }
    return NULL;
}
static GtkWidget *Get(GtkWidget *root, const char *id)
{
    GtkWidget *widget = Find(root, id);
    CHECK(widget);
    return widget;
}
static void Click(GtkWidget *root, const char *id)
{
    g_signal_emit_by_name(Get(root, id), "clicked");
}
static void Set(GtkWidget *root, const char *text)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root, "captions.editor"))),
                             text, -1);
}
static char *Read(GtkWidget *root)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root, "captions.editor")));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
/* Completion is observed through the actual worker state in the UI. Pumping
 * GTK here is a test action; production never blocks its owner thread waiting. */
static void Wait(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (gtk_widget_get_sensitive(Get(root, "captions.stop")) &&
           g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000);
    }
    CHECK(!gtk_widget_get_sensitive(Get(root, "captions.stop")));
    CHECK(gtk_widget_get_sensitive(Get(root, "captions.form")));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    const char *mode = argv[1];
    bool opening = !strncmp(mode, "open", 4);
    GError *error = NULL;
    char *directory = g_dir_make_tmp("umicom-captions-XXXXXX", &error);
    CHECK(directory && !error);
    char *path = g_build_filename(directory, "captions.srt", NULL);
    if (opening || !strcmp(mode, "save-existing"))
        CHECK(g_file_set_contents(path, !strcmp(mode, "open-invalid") ? "broken" : input, -1,
                                  &error));
    GtkWidget *root = NULL;
    CHECK(UmiCaptionsGtkCreate(&root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    Set(root, draft);
    Click(root, "captions.prepare");
    gtk_editable_set_text(GTK_EDITABLE(Get(root, "captions.path")), path);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Get(root, "captions.approval")), TRUE);
    Click(root, opening                     ? "captions.open"
                : !strcmp(mode, "save-vtt") ? "captions.save-vtt"
                                            : "captions.save-srt");
    CHECK(gtk_widget_get_sensitive(Get(root, "captions.stop")));
    if (!strcmp(mode, "open-stale"))
        Set(root, "Edited while reading");
    if (!strcmp(mode, "open-cancel"))
        Click(root, "captions.stop");
    Wait(root);
    char *actual = Read(root);
    if (!strcmp(mode, "open"))
        CHECK(!strcmp(actual, input));
    else if (!strcmp(mode, "open-stale"))
        CHECK(!strcmp(actual, "Edited while reading"));
    else
        CHECK(!strcmp(actual, draft));
    g_free(actual);
    /* Saving to an existing path must preserve its exact bytes. Successful
     * WebVTT output is checked independently of the SRT import path. */
    if (!opening)
    {
        gchar *bytes = NULL;
        gsize length = 0;
        CHECK(g_file_get_contents(path, &bytes, &length, &error));
        const char *expected = !strcmp(mode, "save-existing") ? input
                               : !strcmp(mode, "save-vtt")
                                   ? "WEBVTT\n\n1\n00:00:03.000 --> 00:00:04.000\nMy draft\n\n"
                                   : draft;
        CHECK(length == strlen(expected) && !memcmp(bytes, expected, length));
        g_free(bytes);
    }
    g_object_unref(root);
    /* These paths were created in this test's unique temporary directory. */
    CHECK(g_remove(path) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(path);
    g_free(directory);
    g_clear_error(&error);
    return 0;
}
