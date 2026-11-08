/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_brief_gtk4.c
 *
 * PURPOSE:
 *   Exercise native source preparation, guarded prompt transfer and retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/media_brief.h"
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/media_generation/pixverse_gtk4.h"
#include "umicom/media_generation/seedream_gtk4.h"
#include "umicom/provider_connections/gtk4.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && !strcmp(tag, id)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *id)
{ GtkWidget *button = Find(root, id); CHECK(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked"); }
static void Text(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *view = Find(root, id); CHECK(GTK_IS_TEXT_VIEW(view));
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1);
}
static char *Read(GtkWidget *root, const char *id)
{
    GtkWidget *view = Find(root, id); CHECK(GTK_IS_TEXT_VIEW(view));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
}
static void Prepare(GtkWidget *root)
{
    gtk_editable_set_text(GTK_EDITABLE(Find(root, "media.brief.title")), "Coastal journey");
    Text(root, "media.brief.source", "A train follows the coast.");
    Click(root, "media.brief.prepare");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    if (!strcmp(name, "model-path")) {
        /* Rejected paths and malformed context must return before a storage
         * worker or native window is created. No model or network is needed. */
        UmiProviderConnectionsGtkConfig config = {"umicom-media", "default", "relative.sqlite3"};
        CHECK(UmiProviderConnectionsGtkPresentAt(NULL, &config, "Brief") == UMI_STATUS_INVALID_ARGUMENT);
        config.database_path = "";
        CHECK(UmiProviderConnectionsGtkPresentAt(NULL, &config, "Brief") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderConnectionsGtkPresentAt(NULL, NULL, "Brief") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderConnectionsGtkPresentAt(NULL, &config, "\xc0\xaf") != UMI_STATUS_OK);
        return 0;
    }
    if (!strcmp(name, "composition")) {
        GtkWidget *original = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        g_object_ref_sink(original);
        UmiCreativeGtkSurface *surface = NULL;
        CHECK(UmiCreativeGtkSurfaceCreate(original, "media", &surface) == UMI_STATUS_OK);
        GtkWidget *root = UmiCreativeGtkSurfaceWidget(surface);
        Click(root, "media.mode.create");
        GtkWidget *pages = Find(root, "creative.surface.pages");
        CHECK(!strcmp(gtk_stack_get_visible_child_name(GTK_STACK(pages)), "creation"));
        CHECK(GTK_IS_PANED(Find(root, "media.brief.split")));
        CHECK(Find(root, "seedream.panel") && Find(root, "pixverse.panel"));
        UmiCreativeGtkSurfaceDestroy(surface); g_object_unref(original);
        return 0;
    }
    GtkWidget *image = NULL, *video = NULL;
    CHECK(UmiSeedreamGtkCreate("umicom-media", "default", &image) == UMI_STATUS_OK);
    CHECK(UmiPixVerseGtkCreate("umicom-media", "default", &video) == UMI_STATUS_OK);
    g_object_ref_sink(image); g_object_ref_sink(video);
    GtkWidget *root = UmiMediaBriefGtkCreate(image, video);
    CHECK(root != NULL); g_object_ref_sink(root);
    CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.models")));
    Prepare(root);
    if (!strcmp(name, "prepare")) {
        char *text = Read(root, "media.brief.prompt");
        CHECK(strstr(text, "A train follows the coast.") && strstr(text, "numbered shots"));
        CHECK(gtk_widget_get_sensitive(Find(root, "media.brief.models")));
        CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.image")));
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(root, "media.brief.database"))), ""));
        g_free(text);
    } else if (!strcmp(name, "stale")) {
        char *before = Read(root, "media.brief.prompt");
        Text(root, "media.brief.source", "A different story");
        CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.models")));
        char *after = Read(root, "media.brief.prompt"); CHECK(!strcmp(before, after));
        g_free(before); g_free(after);
    } else if (!strcmp(name, "stale-export")) {
        /* Editing a source invalidates generation handoffs, but must not strand
         * a reviewed draft. Exercise the real export control and inspect the
         * saved bytes, rather than only checking button sensitivity. */
        const char *draft = "Keep my reviewed words\nA quiet coastal scene.";
        Text(root, "media.brief.prompt", draft);
        Text(root, "media.brief.source", "A different source for the next brief.");
        CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.models")));
        GError *error = NULL;
        char *directory = g_dir_make_tmp("umicom-media-brief-XXXXXX", &error);
        CHECK(directory != NULL && error == NULL);
        char *path = g_build_filename(directory, "reviewed-brief.txt", NULL);
        GtkWidget *export = Find(root, "text.export");
        CHECK(export != NULL);
        /* A collapsed expander owns content outside the rendered child tree.
         * Follow the user action before looking up its entry and Save button. */
        CHECK(GTK_IS_EXPANDER(export));
        gtk_expander_set_expanded(GTK_EXPANDER(export), TRUE);
        CHECK(GTK_IS_EDITABLE(Find(export, "text.export.path")));
        gtk_editable_set_text(GTK_EDITABLE(Find(export, "text.export.path")), path);
        Click(export, "text.export.save");
        gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
        while (UmiTextExportGtk4Busy(export) && g_get_monotonic_time() < deadline) {
            /* Dispatch at most one ready event per iteration; an unrelated
             * recurring source cannot prevent the deadline check. */
            (void)g_main_context_iteration(NULL, FALSE);
            g_usleep(1000U);
        }
        CHECK(!UmiTextExportGtk4Busy(export));
        char *saved = NULL;
        gsize size = 0U;
        CHECK(g_file_get_contents(path, &saved, &size, &error));
        CHECK(error == NULL && size == strlen(draft) && !strcmp(saved, draft));
        CHECK(g_remove(path) == 0 && g_rmdir(directory) == 0);
        g_free(saved); g_free(path); g_free(directory);
    } else if (!strcmp(name, "edited") || !strcmp(name, "replace")) {
        Text(root, "media.brief.prompt", "Keep my reviewed words");
        Text(root, "media.brief.source", "New source");
        if (!strcmp(name, "replace")) gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "media.brief.replace")), TRUE);
        Click(root, "media.brief.prepare");
        char *text = Read(root, "media.brief.prompt");
        CHECK(!strcmp(name, "replace") ? strstr(text, "New source") != NULL : !strcmp(text, "Keep my reviewed words"));
        g_free(text);
    } else if (!strcmp(name, "image") || !strcmp(name, "video")) {
        bool is_image = !strcmp(name, "image");
        GtkWidget *target = is_image ? image : video;
        const char *prompt = is_image ? "seedream.prompt" : "pixverse.prompt";
        const char *action = is_image ? "media.brief.image" : "media.brief.video";
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "media.brief.kind")), is_image ? 1U : 2U);
        Click(root, "media.brief.prepare");
        Text(target, prompt, "Existing generator prompt");
        Click(root, action);
        char *text = Read(target, prompt); CHECK(!strcmp(text, "Existing generator prompt")); g_free(text);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "media.brief.replace")), TRUE);
        Click(root, action);
        text = Read(target, prompt); CHECK(strstr(text, "A train follows the coast.")); g_free(text);
        CHECK(!gtk_widget_get_sensitive(Find(target, is_image ? "seedream.send" : "pixverse.send")));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "media.brief.replace"))));
    } else if (!strcmp(name, "invalid-handoff")) {
        CHECK(UmiSeedreamGtkSetPrompt(image, "Keep image", false) == UMI_STATUS_OK);
        CHECK(UmiPixVerseGtkSetPrompt(video, "Keep video", false) == UMI_STATUS_OK);
        CHECK(UmiSeedreamGtkSetPrompt(image, "\xc0\xaf", true) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPixVerseGtkSetPrompt(video, "bad\x1b", true) == UMI_STATUS_INVALID_ARGUMENT);
        char *text = Read(image, "seedream.prompt"); CHECK(!strcmp(text, "Keep image")); g_free(text);
        text = Read(video, "pixverse.prompt"); CHECK(!strcmp(text, "Keep video")); g_free(text);
    } else if (!strcmp(name, "oversized-brief")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "media.brief.kind")), 1U);
        Click(root, "media.brief.prepare");
        char text[4098]; memset(text, 'x', sizeof(text) - 1U); text[sizeof(text) - 1U] = '\0';
        Text(root, "media.brief.prompt", text);
        Click(root, "media.brief.image");
        char *target = Read(image, "seedream.prompt"); CHECK(target[0] == '\0'); g_free(target);
        char *kept = Read(root, "media.brief.prompt"); CHECK(!strcmp(kept, text)); g_free(kept);
    } else if (!strcmp(name, "retired")) {
        GtkWidget *button = Find(root, "media.brief.prepare"); g_object_ref(button);
        UmiMediaBriefGtkRetire(root);
        CHECK(!gtk_widget_is_sensitive(Find(root, "media.brief.models")));
        g_signal_emit_by_name(button, "clicked");
        g_object_unref(root); root = NULL;
        g_signal_emit_by_name(button, "clicked"); g_object_unref(button);
    } else if (!strcmp(name, "missing")) {
        g_clear_object(&image); g_clear_object(&video);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "media.brief.kind")), 1U);
        Click(root, "media.brief.prepare");
        CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.image")));
    } else CHECK(0);
    if (root != NULL) { UmiMediaBriefGtkRetire(root); g_object_unref(root); }
    g_clear_object(&image); g_clear_object(&video);
    return 0;
}
