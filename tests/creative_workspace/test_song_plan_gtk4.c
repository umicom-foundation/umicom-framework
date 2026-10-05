/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_song_plan_gtk4.c
 * PURPOSE: Check the shared song form without file writes, keys or network access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/song_plan_gtk4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
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
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *b = Find(root, id);
    CHECK(b);
    g_signal_emit_by_name(b, "clicked");
}
static void Fill(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *w = Find(root, id);
    CHECK(w);
    gtk_editable_set_text(GTK_EDITABLE(w), text);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    const char *test = argv[1];
    GtkWidget *root = NULL;
    CHECK(UmiSongPlanGtkCreate(&root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(!gtk_widget_get_sensitive(Find(root, "song-plan.save")));
    if (!strcmp(test, "retained"))
    {
        GtkWidget *button = Find(root, "song-plan.prepare");
        g_object_ref(button);
        g_object_unref(root);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        return 0;
    }
    if (!strcmp(test, "save-empty"))
    {
        Click(root, "song-plan.save");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "song-plan.message"))), "Prepare"));
        g_object_unref(root);
        return 0;
    }
    Fill(root, "song-plan.title", "A song plan");
    GtkTextBuffer *lyrics = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "song-plan.lyrics")));
    gtk_text_buffer_set_text(lyrics, "[00:00.000]A new day\n[00:02.000]Another line", -1);
    Click(root, "song-plan.prepare");
    CHECK(gtk_widget_get_sensitive(Find(root, "song-plan.save")));
    if (!strcmp(test, "path-approval") || !strcmp(test, "draft-approval"))
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "song-plan.approval")), TRUE);
        Fill(root, !strcmp(test, "path-approval") ? "song-plan.path" : "song-plan.title", "Changed");
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "song-plan.approval"))));
        g_object_unref(root);
        return 0;
    }
    if (!strcmp(test, "open-denied"))
    {
        Click(root, "song-plan.open");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "song-plan.message"))), "approve"));
        CHECK(gtk_widget_get_sensitive(Find(root, "song-plan.save")));
    }
    else if (!strcmp(test, "invalidate-title"))
        Fill(root, "song-plan.title", "Edited title");
    else if (!strcmp(test, "invalidate-lyrics"))
        gtk_text_buffer_set_text(lyrics, "[00:00.000]Edited", -1);
    else if (!strcmp(test, "invalidate-tempo"))
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "song-plan.tempo")), 137);
    else
        CHECK(!strcmp(test, "review"));
    if (!strncmp(test, "invalidate-", 11))
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "song-plan.save")));
        CHECK(!gtk_widget_get_sensitive(Find(root, "song-plan.captions")));
    }
    g_object_unref(root);
    return 0;
}
