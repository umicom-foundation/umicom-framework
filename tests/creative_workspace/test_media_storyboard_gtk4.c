/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_storyboard_gtk4.c
 *
 * PURPOSE:
 *   Exercise native scene authoring, library references, guarded handoff and retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/media_storyboard.h"
#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/ui/gtk4/media_brief.h"
#include "umicom/ui/gtk4/creative_library.h"
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{ GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id); CHECK(widget != NULL); return widget; }
static void Settle(void)
{
    gint64 until = g_get_monotonic_time() + 300 * G_TIME_SPAN_MILLISECOND;
    do { (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U); } while (g_get_monotonic_time() < until);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id); CHECK(GTK_IS_BUTTON(button));
    CHECK(gtk_widget_is_sensitive(button)); CHECK(gtk_widget_activate(button)); Settle();
}
static void Entry(GtkWidget *root, const char *id, const char *text)
{ gtk_editable_set_text(GTK_EDITABLE(Find(root, id)), text); }
static void Text(GtkWidget *root, const char *id, const char *text)
{ gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, id))), text, -1); }
static char *Read(GtkWidget *root, const char *id)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, id)));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
}
static void Has(GtkWidget *root, const char *id, const char *expected)
{ char *text = Read(root, id); CHECK(strstr(text, expected) != NULL); g_free(text); }
static void Add(GtkWidget *root, const char *title, const char *story)
{
    Entry(root, "media.storyboard.title", title); Text(root, "media.storyboard.story", story);
    Click(root, "media.storyboard.add");
}
static void Page(GtkWidget *stack, GtkWidget *page, const char *name)
{
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), page);
    gtk_stack_add_named(GTK_STACK(stack), scroll, name);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    GtkWidget *window = gtk_window_new(); g_object_ref_sink(window);
    gtk_window_set_default_size(GTK_WINDOW(window), 1360, 900);
    if (!strcmp(name, "composition")) {
        GtkWidget *original = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(original);
        UmiCreativeGtkSurface *surface = NULL;
        CHECK(UmiCreativeGtkSurfaceCreate(original, "media", &surface) == UMI_STATUS_OK);
        GtkWidget *root = UmiCreativeGtkSurfaceWidget(surface);
        gtk_window_set_child(GTK_WINDOW(window), root); gtk_window_present(GTK_WINDOW(window)); Settle();
        Click(root, "media.mode.storyboard");
        GtkWidget *pages = Find(root, "creative.surface.pages");
        CHECK(!strcmp(gtk_stack_get_visible_child_name(GTK_STACK(pages)), "storyboard"));
        CHECK(GTK_IS_PANED(Find(root, "media.storyboard.split")));
        CHECK(GTK_IS_PANED(Find(root, "media.storyboard.editor-split")));
        Click(root, "media.mode.create");
        CHECK(!strcmp(gtk_stack_get_visible_child_name(GTK_STACK(pages)), "creation"));
        UmiCreativeGtkSurfaceDestroy(surface); gtk_window_destroy(GTK_WINDOW(window));
        g_object_unref(original); g_object_unref(window); return 0;
    }
    GtkWidget *library = UmiCreativeAssetLibraryGtkCreate(NULL); CHECK(library != NULL); g_object_ref_sink(library);
    GtkWidget *brief = UmiMediaBriefGtkCreate(NULL, NULL); CHECK(brief != NULL); g_object_ref_sink(brief);
    GtkWidget *root = UmiMediaStoryboardGtkCreate(library, brief); CHECK(root != NULL); g_object_ref_sink(root);
    GtkWidget *stack = gtk_stack_new(); Page(stack, root, "story"); Page(stack, brief, "brief"); Page(stack, library, "library");
    gtk_window_set_child(GTK_WINDOW(window), stack); gtk_window_present(GTK_WINDOW(window)); Settle();
    Add(root, "Opening", "Follow the coastline at sunset.");
    if (!strcmp(name, "author")) {
        Entry(root, "media.storyboard.title", "Revised opening"); Click(root, "media.storyboard.apply");
        Click(root, "media.storyboard.prepare"); Has(root, "media.storyboard.instructions", "Scene 1: Revised opening");
        CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(Find(root, "media.storyboard.instructions"))));
        CHECK(gtk_widget_is_sensitive(Find(root, "media.storyboard.handoff")));
        /* Export through the shared control proves the visible reviewed text is
         * the saved content; a user-selected path owns all resulting files. */
        char *directory = g_dir_make_tmp("umicom-storyboard-XXXXXX", NULL); CHECK(directory != NULL);
        char *path = g_build_filename(directory, "instructions.txt", NULL);
        GtkWidget *export = Find(root, "text.export"); Entry(export, "text.export.path", path); Click(export, "text.export.save");
        gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
        while (UmiTextExportGtk4Busy(export) && g_get_monotonic_time() < end) { (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U); }
        CHECK(!UmiTextExportGtk4Busy(export)); char *saved = NULL, *shown = Read(root, "media.storyboard.instructions");
        CHECK(g_file_get_contents(path, &saved, NULL, NULL) && !strcmp(saved, shown));
        CHECK(g_remove(path) == 0 && g_rmdir(directory) == 0); g_free(saved); g_free(shown); g_free(path); g_free(directory);
    } else if (!strcmp(name, "reorder")) {
        Add(root, "Closing", "The sun sets."); Click(root, "media.storyboard.up"); Click(root, "media.storyboard.prepare");
        Has(root, "media.storyboard.instructions", "Scene 1: Closing"); Has(root, "media.storyboard.instructions", "Scene 2: Opening");
        Click(root, "media.storyboard.down"); Click(root, "media.storyboard.remove"); Click(root, "media.storyboard.prepare");
        char *text = Read(root, "media.storyboard.instructions"); CHECK(strstr(text, "Scene 1: Opening") && !strstr(text, "Closing")); g_free(text);
    } else if (!strcmp(name, "pending")) {
        Add(root, "Closing", "The sun sets."); Text(root, "media.storyboard.story", "Unapplied changes");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "media.storyboard.scenes")), 0U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Find(root, "media.storyboard.scenes"))) == 1U);
        Has(root, "media.storyboard.story", "Unapplied changes"); Click(root, "media.storyboard.prepare");
        CHECK(!gtk_widget_is_sensitive(Find(root, "media.storyboard.handoff")));
        Click(root, "media.storyboard.discard"); Has(root, "media.storyboard.story", "The sun sets.");
    } else if (!strcmp(name, "invalid")) {
        Click(root, "media.storyboard.prepare"); char *before = Read(root, "media.storyboard.instructions");
        Text(root, "media.storyboard.story", "Show @{missing}"); Click(root, "media.storyboard.apply"); Click(root, "media.storyboard.prepare");
        CHECK(!gtk_widget_is_sensitive(Find(root, "media.storyboard.handoff")));
        char *after = Read(root, "media.storyboard.instructions"); CHECK(!strcmp(before, after)); g_free(before); g_free(after);
        char too_long[1026]; memset(too_long, 'x', sizeof(too_long) - 1U); too_long[sizeof(too_long) - 1U] = 0;
        Text(root, "media.storyboard.story", too_long); Click(root, "media.storyboard.apply");
        Click(root, "media.storyboard.discard"); Has(root, "media.storyboard.story", "Show @{missing}");
    } else if (!strcmp(name, "handoff") || !strcmp(name, "occupied")) {
        if (!strcmp(name, "occupied")) Text(brief, "media.brief.source", "Keep existing source");
        Click(root, "media.storyboard.prepare"); Click(root, "media.storyboard.handoff");
        if (!strcmp(name, "occupied")) {
            Has(brief, "media.brief.source", "Keep existing source");
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "media.storyboard.replace")), TRUE);
            Click(root, "media.storyboard.handoff");
        }
        CHECK(!strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack)), "brief"));
        Has(brief, "media.brief.source", "Follow the coastline at sunset.");
        CHECK(!gtk_widget_is_sensitive(Find(brief, "media.brief.video")));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "media.storyboard.replace"))));
        Click(brief, "media.brief.prepare"); Has(brief, "media.brief.prompt", "Follow the coastline at sunset.");
    } else if (!strcmp(name, "assets") || !strcmp(name, "stale-assets")) {
        char *directory = g_dir_make_tmp("umicom-storyboard-assets-XXXXXX", NULL); CHECK(directory != NULL);
        char *path = g_build_filename(directory, "avatar.svg", NULL);
        const char *svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"16\"><circle cx=\"8\" cy=\"8\" r=\"6\"/></svg>";
        CHECK(g_file_set_contents(path, svg, -1, NULL));
        Click(root, "media.storyboard.library");
        CHECK(!strcmp(gtk_stack_get_visible_child_name(GTK_STACK(stack)), "library"));
        Entry(library, "creative.library.source", path); Entry(library, "creative.library.id", "avatar");
        Entry(library, "creative.library.label", "Guide portrait");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(library, "creative.library.kind")), 1U);
        Click(library, "creative.library.import");
        UmiCreativeAssetLibraryInfo info = {0}; UmiStatus status;
        gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
        do {
            status = UmiCreativeAssetLibraryGtkInspect(library, &info);
            if (status == UMI_STATUS_OK) break;
            (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U);
        } while (g_get_monotonic_time() < end);
        CHECK(status == UMI_STATUS_OK && info.asset_count == 1U);
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "story"); Settle();
        Click(root, "media.storyboard.refresh"); Has(root, "media.storyboard.assets", "@{avatar} — Guide portrait");
        Text(root, "media.storyboard.story", "Let portrait introduce the coastline.");
        GtkTextBuffer *story = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "media.storyboard.story")));
        GtkTextIter first, last;
        gtk_text_buffer_get_iter_at_offset(story, &first, 4);
        gtk_text_buffer_get_iter_at_offset(story, &last, 12);
        gtk_text_buffer_select_range(story, &first, &last);
        Click(root, "media.storyboard.insert-tag");
        Has(root, "media.storyboard.story", "Let @{avatar} introduce the coastline.");
        char full[UMI_MEDIA_STORYBOARD_STORY_CAPACITY];
        memset(full, 'x', sizeof(full) - 1U); full[sizeof(full) - 1U] = 0;
        Text(root, "media.storyboard.story", full); Click(root, "media.storyboard.insert-tag");
        char *unchanged = Read(root, "media.storyboard.story"); CHECK(!strcmp(unchanged, full)); g_free(unchanged);
        Text(root, "media.storyboard.story", "Let @{avatar} introduce the coastline.");
        Click(root, "media.storyboard.apply"); Click(root, "media.storyboard.prepare");
        Has(root, "media.storyboard.instructions", "@{avatar} [image] Guide portrait");
        char *instructions = Read(root, "media.storyboard.instructions"); CHECK(strstr(instructions, directory) == NULL); g_free(instructions);
        if (!strcmp(name, "stale-assets")) {
            Click(root, "media.storyboard.library"); Click(library, "creative.library.remove");
            gtk_stack_set_visible_child_name(GTK_STACK(stack), "story"); Settle();
            Click(root, "media.storyboard.handoff");
            CHECK(!gtk_widget_is_sensitive(Find(root, "media.storyboard.handoff")));
            char *source = Read(brief, "media.brief.source"); CHECK(source[0] == 0); g_free(source);
        } else {
            Click(root, "media.storyboard.handoff"); Has(brief, "media.brief.source", "@{avatar} [image] Guide portrait");
        }
        CHECK(g_remove(path) == 0 && g_rmdir(directory) == 0); g_free(path); g_free(directory);
    } else if (!strcmp(name, "retired")) {
        Click(root, "media.storyboard.prepare"); GtkWidget *button = Find(root, "media.storyboard.handoff"); g_object_ref(button);
        UmiMediaStoryboardGtkRetire(root); CHECK(!gtk_widget_is_sensitive(button));
        /* Direct emission is deliberate only for the adversarial retained-control
         * case; normal workflow checks above activate the actual GTK control. */
        g_signal_emit_by_name(button, "clicked"); char *source = Read(brief, "media.brief.source"); CHECK(source[0] == 0); g_free(source);
        gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(root); root = NULL;
        g_signal_emit_by_name(button, "clicked"); g_object_unref(button);
    } else if (!strcmp(name, "source")) {
        UmiMediaBriefDraft draft = {0}; draft.kind = UMI_MEDIA_BRIEF_VIDEO;
        memcpy(draft.title, "Incoming scene", sizeof("Incoming scene")); memcpy(draft.source, "Source text", sizeof("Source text"));
        CHECK(UmiMediaBriefGtkSetSource(brief, &draft, false) == UMI_STATUS_OK);
        CHECK(UmiMediaBriefGtkSetSource(brief, &draft, false) == UMI_STATUS_ALREADY_EXISTS);
        draft.source[0] = '\x1b';
        CHECK(UmiMediaBriefGtkSetSource(brief, &draft, true) == UMI_STATUS_INVALID_ARGUMENT);
        Has(brief, "media.brief.source", "Source text");
        UmiMediaBriefGtkRetire(brief); CHECK(UmiMediaBriefGtkSetSource(brief, &draft, true) == UMI_STATUS_INVALID_STATE);
    } else CHECK(0);
    if (root != NULL) UmiMediaStoryboardGtkRetire(root);
    UmiMediaBriefGtkRetire(brief); gtk_window_destroy(GTK_WINDOW(window));
    g_clear_object(&root); g_object_unref(brief); g_object_unref(library); g_object_unref(window); return 0;
}
