/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_workspace_gtk4.c
 *
 * PURPOSE:
 *   Verify mapped Media regions, timeline edits, retained controls and native screenshot evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "../../adapters/gtk4/video_sequence_private.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/creative_workspace.h"

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
    CHECK(widget != NULL);
    return widget;
}
static void Click(GtkWidget *root, const char *id)
{
    g_signal_emit_by_name(Find(root, id), "clicked");
}
static void Settle(void)
{
    gint64 deadline = g_get_monotonic_time() + 350000;
    do {
        while (g_main_context_pending(NULL)) (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(2000U);
    } while (g_get_monotonic_time() < deadline);
}
static void Add(GtkWidget *root, const char *id, double end)
{
    gtk_editable_set_text(GTK_EDITABLE(Find(root, "video.id")), id);
    gtk_editable_set_text(GTK_EDITABLE(Find(root, "video.path")), VIDEO_PATH);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "video.begin")), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(root, "video.end")), end);
    Click(root, "video.add");
}
static graphene_rect_t Bounds(GtkWidget *root, const char *id)
{
    GtkWidget *widget = Find(root, id);
    graphene_rect_t bounds;
    CHECK(gtk_widget_get_mapped(widget));
    CHECK(gtk_widget_compute_bounds(widget, root, &bounds));
    CHECK(bounds.size.width > 0.0F && bounds.size.height > 0.0F);
    return bounds;
}
static void Capture(GtkWidget *window, GtkWidget *root)
{
    /* Capture actual mapped GTK content after allocation. This does not imply
     * that playback codecs or an external encoder have been qualified. */
    GskRenderer *renderer = gtk_native_get_renderer(GTK_NATIVE(window));
    CHECK(renderer != NULL);
    GdkPaintable *paintable = gtk_widget_paintable_new(root);
    GtkSnapshot *snapshot = gtk_snapshot_new();
    gdk_paintable_snapshot(paintable, GDK_SNAPSHOT(snapshot),
        gtk_widget_get_width(root), gtk_widget_get_height(root));
    GskRenderNode *node = gtk_snapshot_free_to_node(snapshot);
    CHECK(node != NULL);
    GdkTexture *texture = gsk_renderer_render_texture(renderer, node, NULL);
    CHECK(texture != NULL);
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(file, directory, "media-workspace.png");
    CHECK(gdk_texture_save_to_png(texture, file));
    (void)printf("Native Media screenshot: %s\n", file);
    g_object_unref(texture); gsk_render_node_unref(node); g_object_unref(paintable);
}
static void Geometry(GtkWidget *root)
{
    graphene_rect_t sources = Bounds(root, "video.sources");
    graphene_rect_t preview = Bounds(root, "video.preview-region");
    graphene_rect_t inspector = Bounds(root, "video.inspector");
    graphene_rect_t timeline = Bounds(root, "video.timeline");
    CHECK(sources.origin.x + sources.size.width <= preview.origin.x + 1.0F);
    CHECK(preview.origin.x + preview.size.width <= inspector.origin.x + 1.0F);
    CHECK(preview.size.width >= 240.0F && preview.size.height >= 180.0F);
    CHECK(timeline.origin.y >= preview.origin.y + preview.size.height - 1.0F);
    CHECK(timeline.origin.y + timeline.size.height <= (float)gtk_widget_get_height(root) + 1.0F);
    CHECK(gtk_widget_get_mapped(Find(root, "video.add")));
    CHECK(gtk_widget_get_mapped(Find(root, "video.preview")));
    CHECK(gtk_widget_get_mapped(Find(root, "video.timeline.clip.0")));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    CHECK(strcmp(mode, "geometry") == 0 || strcmp(mode, "selection") == 0 ||
          strcmp(mode, "edits") == 0 || strcmp(mode, "retirement") == 0 ||
          strcmp(mode, "composition") == 0 || strcmp(mode, "music-composition") == 0);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check()) return 77;
    GtkWidget *window = gtk_window_new();
    gtk_window_set_default_size(GTK_WINDOW(window), 1280, 800);
    UmiCreativeGtkSurface *surface = NULL;
    GtkWidget *original = NULL, *host, *root;
    if (strcmp(mode, "composition") == 0 || strcmp(mode, "music-composition") == 0) {
        bool music = strcmp(mode, "music-composition") == 0;
        original = gtk_label_new("Original product layout"); g_object_ref_sink(original);
        CHECK(UmiCreativeGtkSurfaceCreate(original, music ? "music" : "media", &surface) == UMI_STATUS_OK);
        host = UmiCreativeGtkSurfaceWidget(surface);
        root = Find(host, "video-sequence"); g_object_ref(root);
        GtkStack *pages = GTK_STACK(Find(host, "creative.surface.pages"));
        CHECK(strcmp(gtk_stack_get_visible_child_name(pages), music ? "creative-workspace" : "video-editing") == 0);
        if (music) gtk_stack_set_visible_child_name(pages, "video-editing");
        CHECK(gtk_stack_get_child_by_name(pages, "video-editing") == root);
    } else {
        root = UmiVideoSequenceGtk4Create(); CHECK(root != NULL);
        g_object_ref_sink(root); host = root;
    }
    UmiVideoSequencePanel *panel = UmiVideoSequenceGtkState(root);
    gtk_window_set_child(GTK_WINDOW(window), host);
    gtk_window_present(GTK_WINDOW(window)); Settle();
    CHECK(gtk_widget_get_mapped(root));
    Add(root, "first", 5000.0); Add(root, "second", 3000.0); Settle();
    CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 2U);
    GtkWidget *first = Find(root, "video.timeline.clip.0");
    GtkWidget *second = Find(root, "video.timeline.clip.1");
    if (strcmp(mode, "geometry") == 0 || strcmp(mode, "composition") == 0 ||
        strcmp(mode, "music-composition") == 0) {
        Geometry(root); Capture(window, host);
        gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768); Settle();
        Geometry(root); Capture(window, host);
        CHECK(Find(root, "video.timeline.clip.0") == first);
        CHECK(Find(root, "video.timeline.clip.1") == second);
    }
    if (strcmp(mode, "selection") == 0) {
        UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
        bool dirty = panel->dirty;
        CHECK(gtk_widget_grab_focus(second));
        Click(root, "video.timeline.clip.1");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(panel->id)), "second") == 0);
        CHECK(gtk_widget_has_css_class(second, "suggested-action"));
        CHECK(!gtk_widget_has_css_class(first, "suggested-action"));
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
        CHECK(panel->dirty == dirty);
        gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768); Settle();
        CHECK(gtk_widget_has_focus(second));
        /* Selection must not consume an undo slot: undo removes the last add. */
        Click(root, "video.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 1U);
    }
    if (strcmp(mode, "edits") == 0) {
        Click(root, "video.timeline.clip.1");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->position), 1.0);
        Click(root, "video.move");
        CHECK(strcmp(UmiVideoSequenceEditorRead(panel->editor)->clips[0].id, "second") == 0);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->selector)) == 0U);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->split), 1000.0);
        gtk_editable_set_text(GTK_EDITABLE(panel->rightId), "second-right");
        Click(root, "video.split");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 3U);
        Click(root, "video.timeline.clip.1"); Click(root, "video.remove");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 2U);
        Click(root, "video.undo");
        CHECK(UmiVideoSequenceEditorRead(panel->editor)->clip_count == 3U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->durationLabel)), "3 clips | 8.00 seconds") != NULL);
    }
    if (strcmp(mode, "retirement") == 0) {
        UmiVideoSequence before = *UmiVideoSequenceEditorRead(panel->editor);
        UmiVideoSequenceGtk4Retire(root);
        CHECK(panel->closed);
        Click(root, "video.timeline.clip.1"); Click(root, "video.remove");
        CHECK(UmiVideoSequenceEqual(&before, UmiVideoSequenceEditorRead(panel->editor)));
    }
    gtk_window_set_child(GTK_WINDOW(window), NULL);
    if (surface != NULL) {
        UmiCreativeGtkSurfaceDestroy(surface);
        CHECK(panel->closed);
        CHECK(gtk_widget_get_parent(original) == NULL);
        g_object_unref(original);
    } else UmiVideoSequenceGtk4Retire(root);
    gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(root);
    return 0;
}
