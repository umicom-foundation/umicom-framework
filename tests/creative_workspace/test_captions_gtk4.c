/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_captions_gtk4.c
 * PURPOSE: Check shared caption controls, stale drafts, undo and retained widget ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/captions.h"
#include "umicom/ui/gtk4/creative_workspace.h"
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
static const char text[] = "1\n00:00:01,000 --> 00:00:04,000\nFirst\n\n"
                           "2\n00:00:03,000 --> 00:00:05,000\nSecond\n\n";
/* Stable identifiers avoid coupling tests to button translations or GTK child
 * implementation details. Media and Music must expose the same controls. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag && !strcmp(tag, id))
        return root;
    for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c))
    {
        GtkWidget *found = Find(c, id);
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
static char *Read(GtkWidget *view)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
static void Set(GtkWidget *root, const char *text_value)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Get(root, "captions.editor"))),
                             text_value, -1);
}
/* A host can edit programmatically during a widget update. The prepared
 * capture must then be discarded rather than enabling export of stale text. */
static void Interfere(GtkTextBuffer *buffer, gpointer data)
{
    bool *done = data;
    if (!*done)
    {
        *done = true;
        gtk_text_buffer_set_text(buffer, "Host changed this draft", -1);
    }
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    if (!strcmp(argv[1], "media") || !strcmp(argv[1], "music"))
    {
        UmiDataServer *server = NULL;
        UmiCreativeGtkPanel *panel = NULL;
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        CHECK(UmiCreativeGtkPanelCreate(server, argv[1], &panel) == UMI_STATUS_OK);
        GtkWidget *root = UmiCreativeGtkPanelWidget(panel);
        CHECK(Find(root, "captions.panel") && Find(root, "song-plan.panel"));
        CHECK(umi_data_server_count(server) == 0U);
        UmiCreativeGtkPanelDestroy(panel);
        umi_data_server_destroy(server);
        return 0;
    }
    GtkWidget *root = NULL;
    CHECK(UmiCaptionsGtkCreate(&root) == UMI_STATUS_OK && root);
    g_object_ref_sink(root);
    GtkWidget *editor = Get(root, "captions.editor"), *output = Get(root, "captions.output");
    Set(root, text);
    Click(root, "captions.prepare");
    const char *mode = argv[1];
    /* Pasted WebVTT follows the same preparation path as a file import. A
     * rejected styled draft stays visible and cannot export an older capture. */
    if (!strcmp(mode, "prepare-vtt") || !strcmp(mode, "invalid-vtt"))
    {
        bool invalid = !strcmp(mode, "invalid-vtt");
        const char *input = invalid
            ? "WEBVTT\n\n00:01.000 --> 00:02.000 align:start\nKeep this draft\n"
            : "WEBVTT\n\n00:01.000 --> 00:04.000\nFirst\n\n00:03.000 --> 00:05.000\nSecond\n\n";
        Set(root, input);
        Click(root, "captions.prepare");
        char *actual = Read(editor);
        CHECK(!strcmp(actual, invalid ? input : text));
        g_free(actual);
        if (invalid)
        {
            Click(root, "captions.save-vtt");
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root, "captions.message"))), "Prepare"));
        }
        else
        {
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.time")), 3500);
            Click(root, "captions.preview");
            actual = Read(output);
            CHECK(!strcmp(actual, "First\n\nSecond"));
            g_free(actual);
        }
        g_object_unref(root);
        return 0;
    }
    if (!strcmp(mode, "prepare"))
    {
        char *actual = Read(editor);
        CHECK(!strcmp(actual, text));
        g_free(actual);
    }
    else if (!strcmp(mode, "preview") || !strcmp(mode, "overlap"))
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.time")),
                                  !strcmp(mode, "overlap") ? 3500 : 2000);
        Click(root, "captions.preview");
        char *actual = Read(output);
        CHECK(!strcmp(actual, !strcmp(mode, "overlap") ? "First\n\nSecond" : "First"));
        g_free(actual);
    }
    else if (!strcmp(mode, "shift") || !strcmp(mode, "shift-rejected"))
    {
        bool rejected = !strcmp(mode, "shift-rejected");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.offset")),
                                  rejected ? -1001 : 1000);
        Click(root, "captions.shift");
        char *actual = Read(editor);
        CHECK(rejected ? !strcmp(actual, text)
                       : strstr(actual, "00:00:02,000 --> 00:00:05,000") != NULL);
        g_free(actual);
    }
    else if (!strcmp(mode, "clip"))
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.begin")), 3000);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.end")), 4000);
        Click(root, "captions.clip");
        char *actual = Read(editor);
        CHECK(!strcmp(actual, "1\n00:00:00,000 --> 00:00:01,000\nFirst\n\n"
                              "2\n00:00:00,000 --> 00:00:01,000\nSecond\n\n"));
        g_free(actual);
    }
    else if (!strcmp(mode, "undo"))
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.offset")), 1000);
        Click(root, "captions.shift");
        Click(root, "captions.undo");
        char *actual = Read(editor);
        CHECK(!strcmp(actual, text));
        g_free(actual);
        Click(root, "captions.undo");
        actual = Read(editor);
        CHECK(strstr(actual, "00:00:02,000") != NULL);
        g_free(actual);
    }
    else if (!strcmp(mode, "invalidate") || !strcmp(mode, "invalid-draft"))
    {
        Set(root, "invalid");
        Click(root, !strcmp(mode, "invalid-draft") ? "captions.prepare" : "captions.preview");
        char *actual = Read(editor);
        CHECK(!strcmp(actual, "invalid"));
        g_free(actual);
        actual = Read(output);
        CHECK(!*actual);
        g_free(actual);
        Click(root, "captions.save-srt");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root, "captions.message"))), "Prepare") !=
              NULL);
    }
    else if (!strcmp(mode, "open-denied"))
    {
        Click(root, "captions.open");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root, "captions.message"))), "approve") !=
              NULL);
        char *actual = Read(editor);
        CHECK(!strcmp(actual, text));
        g_free(actual);
    }
    else if (!strcmp(mode, "retained"))
    {
        GtkWidget *button = Get(root, "captions.prepare");
        g_object_ref(button);
        g_object_unref(root);
        root = NULL;
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
    }
    else if (!strcmp(mode, "reentrant"))
    {
        bool changed = false;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor));
        gulong handler = g_signal_connect(buffer, "changed", G_CALLBACK(Interfere), &changed);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Get(root, "captions.offset")), 1000);
        Click(root, "captions.shift");
        CHECK(changed);
        g_signal_handler_disconnect(buffer, handler);
        Click(root, "captions.save-srt");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Get(root, "captions.message"))), "Prepare") !=
              NULL);
    }
    else
        CHECK(false);
    if (root)
        g_object_unref(root);
    return 0;
}
