/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_heygen_gtk4.c
 * PURPOSE: Check native review invalidation and retained-control lifetime without network or vault access.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media_generation/heygen_gtk4.h"
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
    if (tag && strcmp(tag, id) == 0)
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
    CHECK(UmiHeyGenGtkCreate("umicom-media", "default", &root) == UMI_STATUS_OK);
    g_object_ref_sink(root);
    CHECK(Find(root, "heygen.avatar") && Find(root, "heygen.voice") && Find(root, "heygen.catalogue"));
    CHECK(!gtk_widget_get_sensitive(Find(root, "heygen.send")));
    if (strcmp(test, "retained") == 0)
    {
        GtkWidget *button = Find(root, "heygen.prepare-video");
        g_object_ref(button);
        g_object_unref(root);
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(button);
        return 0;
    }
    Fill(root, "heygen.name", "Tutorial");
    Fill(root, "heygen.avatar", "look-1");
    Fill(root, "heygen.voice", "voice-1");
    GtkTextBuffer *script = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "heygen.script")));
    gtk_text_buffer_set_text(script, "A calm tutorial presenter", -1);
    bool scene = strncmp(test, "scene", 5U) == 0;
    if (strcmp(test, "scene-image") == 0)
        Fill(root, "heygen.image-asset", "asset-1");
    Click(root, scene                            ? "heygen.prepare-scene"
                : strcmp(test, "character") == 0 ? "heygen.prepare-avatar"
                                                 : "heygen.prepare-video");
    CHECK(gtk_widget_get_sensitive(Find(root, "heygen.send")));
    GtkTextBuffer *review = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "heygen.review")));
    GtkTextIter a, b;
    gtk_text_buffer_get_bounds(review, &a, &b);
    char *text = gtk_text_buffer_get_text(review, &a, &b, FALSE);
    CHECK(strstr(text, "https://api.heygen.com/") &&
          strstr(text, scene || strcmp(test, "character") == 0 ? "\"prompt\"" : "look-1"));
    CHECK(strstr(text, "Local key alias: heygen") && strstr(text, "umicom-media / default"));
    g_free(text);
    if (strcmp(test, "unapproved") == 0)
    {
        Click(root, "heygen.send");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "heygen.message"))), "approve") != NULL);
    }
    else if (strcmp(test, "edit") == 0)
        Fill(root, "heygen.name", "Changed title");
    else if (strcmp(test, "password") == 0)
        Fill(root, "heygen.password", "fixture-password-only");
    else if (strcmp(test, "alias") == 0)
        Fill(root, "heygen.alias", "another-heygen-account");
    else if (strcmp(test, "private-filter") == 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "heygen.private")), TRUE);
    else if (strcmp(test, "script") == 0)
        gtk_text_buffer_set_text(script, "A different script", -1);
    else if (strcmp(test, "portrait") == 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "heygen.portrait")), TRUE);
    else if (strcmp(test, "scene-duration") == 0)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "heygen.scene-seconds")), 10.0);
    else if (strcmp(test, "scene-asset-edit") == 0)
        Fill(root, "heygen.image-asset", "asset-2");
    else
        CHECK(strcmp(test, "review") == 0 || strcmp(test, "character") == 0 || strcmp(test, "scene") == 0 ||
              strcmp(test, "scene-image") == 0);
    if (strcmp(test, "edit") == 0 || strcmp(test, "password") == 0 || strcmp(test, "script") == 0 ||
        strcmp(test, "alias") == 0 || strcmp(test, "private-filter") == 0 || strcmp(test, "portrait") == 0 ||
        strcmp(test, "scene-duration") == 0 || strcmp(test, "scene-asset-edit") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(Find(root, "heygen.send")));
        CHECK(gtk_text_buffer_get_char_count(review) == 0);
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "heygen.approval"))));
    }
    g_object_unref(root);
    return 0;
}
