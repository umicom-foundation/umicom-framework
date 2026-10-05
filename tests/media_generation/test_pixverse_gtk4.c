/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_pixverse_gtk4.c
 * PURPOSE: Exercise review invalidation and closed-panel controls without a provider request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/pixverse_gtk4.h"
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
    CHECK(UmiPixVerseGtkCreate("umicom-media", "default", &root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(!gtk_widget_get_sensitive(Find(root, "pixverse.send")));
    if (!strcmp(test, "retained"))
    {
        GtkWidget *button = Find(root, "pixverse.prepare");
        g_object_ref(button);
        g_object_unref(root);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        return 0;
    }
    Fill(root, "pixverse.password", "fixture-private-password");
    GtkTextBuffer *prompt = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "pixverse.prompt")));
    gtk_text_buffer_set_text(prompt, "A quiet sea at sunrise", -1);
    bool status_check = !strncmp(test, "status", 6);
    if (status_check)
        Fill(root, "pixverse.identity", !strcmp(test, "status-invalid") ? "42/path" : "42");
    Click(root, status_check ? "pixverse.status" : "pixverse.prepare");
    if (!strcmp(test, "status-invalid"))
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "pixverse.send")));
        g_object_unref(root);
        return 0;
    }
    CHECK(gtk_widget_get_sensitive(Find(root, "pixverse.send")));
    GtkTextBuffer *review = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "pixverse.review")));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(review, &first, &last);
    char *text = gtk_text_buffer_get_text(review, &first, &last, FALSE);
    CHECK(strstr(text, "app-api.pixverse.ai") && !strstr(text, "fixture-private-password"));
    CHECK(status_check ? strstr(text, "video/result/42") != NULL : strstr(text, "A quiet sea") != NULL);
    g_free(text);
    if (strcmp(test, "unapproved"))
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "pixverse.approval")), TRUE);
    if (!strcmp(test, "unapproved"))
    {
        Click(root, "pixverse.send");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "pixverse.message"))), "approve"));
    }
    else if (!strcmp(test, "alias"))
        Fill(root, "pixverse.alias", "another-key");
    else if (!strcmp(test, "password"))
        Fill(root, "pixverse.password", "another-password");
    else if (!strcmp(test, "prompt"))
        gtk_text_buffer_set_text(prompt, "Edited", -1);
    else if (!strcmp(test, "ratio"))
        Fill(root, "pixverse.ratio", "9:16");
    else if (!strcmp(test, "seconds"))
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "pixverse.seconds")), 7);
    else if (!strcmp(test, "quality"))
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "pixverse.quality")), 1080);
    else if (!strcmp(test, "audio"))
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "pixverse.audio")), TRUE);
    else if (!strcmp(test, "shots"))
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "pixverse.shots")), TRUE);
    else
        CHECK(!strcmp(test, "review") || !strcmp(test, "status"));
    if (strcmp(test, "review") && strcmp(test, "status") && strcmp(test, "unapproved"))
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "pixverse.send")));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "pixverse.approval"))));
        CHECK(gtk_text_buffer_get_char_count(review) == 0);
    }
    g_object_unref(root);
    return 0;
}
