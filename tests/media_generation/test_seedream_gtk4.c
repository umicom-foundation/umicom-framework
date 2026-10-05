/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_seedream_gtk4.c
 * PURPOSE: Check native review invalidation and retained controls without sending requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/seedream_gtk4.h"
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
    CHECK(UmiSeedreamGtkCreate("umicom-media", "default", &root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(Find(root, "seedream.preview") && Find(root, "seedream.path"));
    CHECK(!gtk_widget_get_sensitive(Find(root, "seedream.send")));
    CHECK(!gtk_widget_get_sensitive(Find(root, "seedream.save")));
    if (!strcmp(test, "retained"))
    {
        GtkWidget *b = Find(root, "seedream.prepare");
        g_object_ref(b);
        g_object_unref(root);
        g_signal_emit_by_name(b, "clicked");
        g_object_unref(b);
        return 0;
    }
    if (!strcmp(test, "save-empty"))
    {
        Click(root, "seedream.save");
        CHECK(
            strstr(gtk_label_get_text(GTK_LABEL(Find(root, "seedream.message"))), "Generate an image first"));
        g_object_unref(root);
        return 0;
    }
    Fill(root, "seedream.model", "fixture-model");
    Fill(root, "seedream.password", "fixture-private-password");
    GtkTextBuffer *prompt = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "seedream.prompt")));
    gtk_text_buffer_set_text(prompt, "A tutorial title card", -1);
    Click(root, "seedream.prepare");
    CHECK(gtk_widget_get_sensitive(Find(root, "seedream.send")));
    GtkTextBuffer *review = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "seedream.review")));
    GtkTextIter a, b;
    gtk_text_buffer_get_bounds(review, &a, &b);
    char *text = gtk_text_buffer_get_text(review, &a, &b, FALSE);
    CHECK(strstr(text, "ark.ap-southeast.bytepluses.com") && strstr(text, "byteplus-modelark"));
    CHECK(strstr(text, "A tutorial title card") && !strstr(text, "fixture-private-password"));
    g_free(text);
    if (!strcmp(test, "unapproved"))
    {
        Click(root, "seedream.send");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "seedream.message"))), "approve"));
    }
    else if (!strcmp(test, "model"))
        Fill(root, "seedream.model", "another-model");
    else if (!strcmp(test, "password"))
        Fill(root, "seedream.password", "another-password");
    else if (!strcmp(test, "alias"))
        Fill(root, "seedream.alias", "another-account");
    else if (!strcmp(test, "prompt"))
        gtk_text_buffer_set_text(prompt, "Changed prompt", -1);
    else
        CHECK(!strcmp(test, "review"));
    if (strcmp(test, "review") && strcmp(test, "unapproved"))
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "seedream.send")));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "seedream.approval"))));
        CHECK(gtk_text_buffer_get_char_count(review) == 0);
    }
    g_object_unref(root);
    return 0;
}
