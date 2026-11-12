/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_storyboard_recovery_gtk4.c
 *
 * PURPOSE:
 *   Exercise native editable-scene file controls, pending edits and retired or stale worker completions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/media_storyboard.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/creative_workspace/media_storyboard.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
    CHECK(widget != NULL);
    return widget;
}
static void Pump(unsigned milliseconds)
{
    gint64 end = g_get_monotonic_time() + (gint64)milliseconds * G_TIME_SPAN_MILLISECOND;
    do { (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U); }
    while (g_get_monotonic_time() < end);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    CHECK(GTK_IS_BUTTON(button) && gtk_widget_get_mapped(button) && gtk_widget_is_sensitive(button));
    CHECK(gtk_widget_activate(button));
    Pump(300U);
}
static void SetEntry(GtkWidget *root, const char *id, const char *text)
{ gtk_editable_set_text(GTK_EDITABLE(Find(root, id)), text); }
static void SetStory(GtkWidget *root, const char *text)
{ gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "media.storyboard.story"))), text, -1); }
static const char *Title(GtkWidget *root)
{ return gtk_editable_get_text(GTK_EDITABLE(Find(root, "media.storyboard.title"))); }
static size_t SceneCount(GtkWidget *root)
{ return g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(Find(root, "media.storyboard.scenes")))); }
static void Approve(GtkWidget *root, bool opening)
{ gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, opening ? "media.storyboard.file.allow-open" : "media.storyboard.file.allow-save")), TRUE); }
static bool Approved(GtkWidget *root, bool opening)
{ return gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, opening ? "media.storyboard.file.allow-open" : "media.storyboard.file.allow-save"))); }
static void Wait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiMediaStoryboardGtkBusy(root) && g_get_monotonic_time() < end) {
        (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U);
    }
    CHECK(!UmiMediaStoryboardGtkBusy(root));
}
static UmiMediaStoryboardDraft Incoming(bool empty)
{
    UmiMediaStoryboardDraft draft = {0};
    if (!empty) {
        draft.count = 1U;
        strcpy(draft.scenes[0].title, "From disk");
        strcpy(draft.scenes[0].story, "Unresolved @{harbour} remains editable.");
        draft.scenes[0].duration_seconds = 15U;
    }
    return draft;
}
static void WriteIncoming(const char *path, bool empty)
{
    UmiMediaStoryboardDraft draft = Incoming(empty);
    CHECK(UmiMediaStoryboardDraftSave(&draft, path, NULL) == UMI_STATUS_OK);
}
/* Emit a semantic action synchronously only for deterministic interleaving tests.
 * No main-context iteration occurs before direct mutation/retirement, so a
 * worker completion cannot install its result first. Other cases use mapped
 * native activation. This is not pointer-input or visual qualification. */
static void StartOpen(GtkWidget *root)
{
    Approve(root, true);
    g_signal_emit_by_name(Find(root, "media.storyboard.file.open"), "clicked");
    CHECK(UmiMediaStoryboardGtkBusy(root));
    CHECK(!Approved(root, true) && !Approved(root, false));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    char *directory = g_dir_make_tmp("umicom-storyboard-recovery-XXXXXX", NULL);
    CHECK(directory != NULL);
    char *path = g_build_filename(directory, "scenes.json", NULL);
    GtkWidget *window = gtk_window_new(); g_object_ref_sink(window);
    gtk_window_set_default_size(GTK_WINDOW(window), 1300, 1000);
    GtkWidget *root = UmiMediaStoryboardGtkCreate(NULL, NULL); CHECK(root != NULL);
    g_object_ref_sink(root);
    gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_window_present(GTK_WINDOW(window)); Pump(300U);
    SetEntry(root, "media.storyboard.title", "Current");
    SetStory(root, "The current scene.");
    Click(root, "media.storyboard.add");
    GtkWidget *files = Find(root, "media.storyboard.file");
    CHECK(GTK_IS_EXPANDER(files));
    gtk_expander_set_expanded(GTK_EXPANDER(files), TRUE); Pump(300U);
    SetEntry(root, "media.storyboard.file.path", path);

    if (strcmp(name, "save-open") == 0 || strcmp(name, "unicode") == 0 ||
        strcmp(name, "save-replacement") == 0) {
        if (strcmp(name, "unicode") == 0) {
            SetEntry(root, "media.storyboard.title", "Caf\xc3\xa9");
            SetStory(root, "Dawn \xe2\x98\x80 over the harbour.");
            Click(root, "media.storyboard.apply");
        }
        if (strcmp(name, "save-replacement") == 0) WriteIncoming(path, false);
        char *expected_title = g_strdup(Title(root));
        Approve(root, false); Click(root, "media.storyboard.file.save"); Wait(root);
        CHECK(!Approved(root, false));
        UmiMediaStoryboardDraft saved = {0};
        CHECK(UmiMediaStoryboardDraftLoad(path, NULL, &saved) == UMI_STATUS_OK);
        CHECK(saved.count == 1U && strcmp(saved.scenes[0].title, expected_title) == 0);
        SetEntry(root, "media.storyboard.title", "Changed afterward");
        Click(root, "media.storyboard.apply");
        Approve(root, true); Click(root, "media.storyboard.file.open"); Wait(root);
        CHECK(strcmp(Title(root), expected_title) == 0 && SceneCount(root) == 1U);
        CHECK(!gtk_widget_is_sensitive(Find(root, "media.storyboard.handoff")));
        g_free(expected_title);
    } else if (strcmp(name, "pending-save") == 0 || strcmp(name, "pending-open") == 0) {
        bool opening = strcmp(name, "pending-open") == 0;
        if (opening) WriteIncoming(path, false);
        SetEntry(root, "media.storyboard.title", "Unapplied");
        Approve(root, opening);
        Click(root, opening ? "media.storyboard.file.open" : "media.storyboard.file.save");
        CHECK(!UmiMediaStoryboardGtkBusy(root));
        CHECK(strcmp(Title(root), "Unapplied") == 0);
        if (!opening) CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    } else if (strcmp(name, "save-approval") == 0 || strcmp(name, "open-approval") == 0) {
        bool opening = strcmp(name, "open-approval") == 0;
        if (opening) WriteIncoming(path, false);
        Click(root, opening ? "media.storyboard.file.open" : "media.storyboard.file.save");
        CHECK(!UmiMediaStoryboardGtkBusy(root) && strcmp(Title(root), "Current") == 0);
        if (!opening) CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    } else if (strcmp(name, "path-change-reset") == 0) {
        Approve(root, false); Approve(root, true);
        SetEntry(root, "media.storyboard.file.path", directory);
        CHECK(!Approved(root, false) && !Approved(root, true));
    } else if (strcmp(name, "edit-resets-approval") == 0) {
        Approve(root, false); Approve(root, true);
        SetStory(root, "A pending change.");
        CHECK(!Approved(root, false) && !Approved(root, true));
        Click(root, "media.storyboard.discard");
        Approve(root, false); Approve(root, true);
        Click(root, "media.storyboard.add");
        CHECK(!Approved(root, false) && !Approved(root, true));
    } else if (strcmp(name, "damaged-open") == 0) {
        CHECK(g_file_set_contents(path, "{", 1, NULL));
        Approve(root, true); Click(root, "media.storyboard.file.open"); Wait(root);
        CHECK(strcmp(Title(root), "Current") == 0 && SceneCount(root) == 1U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "media.storyboard.message"))), "failed") != NULL);
    } else if (strcmp(name, "empty-open") == 0) {
        WriteIncoming(path, true);
        Approve(root, true); Click(root, "media.storyboard.file.open"); Wait(root);
        CHECK(SceneCount(root) == 0U && strcmp(Title(root), "") == 0);
    } else if (strcmp(name, "stale-open") == 0 || strcmp(name, "busy-action") == 0 ||
        strcmp(name, "retire-open") == 0) {
        WriteIncoming(path, false);
        StartOpen(root);
        if (strcmp(name, "stale-open") == 0) {
            SetEntry(root, "media.storyboard.title", "Newer direct edit");
            Wait(root);
            CHECK(strcmp(Title(root), "Newer direct edit") == 0 && SceneCount(root) == 1U);
        } else if (strcmp(name, "busy-action") == 0) {
            CHECK(SceneCount(root) == 1U);
            g_signal_emit_by_name(Find(root, "media.storyboard.add"), "clicked");
            CHECK(SceneCount(root) == 1U);
            Wait(root);
            CHECK(strcmp(Title(root), "From disk") == 0);
        } else {
            UmiMediaStoryboardGtkRetire(root);
            char *message = g_strdup(gtk_label_get_text(GTK_LABEL(Find(root, "media.storyboard.message"))));
            Wait(root);
            CHECK(!gtk_widget_is_sensitive(root) && strcmp(Title(root), "Current") == 0);
            CHECK(strcmp(message, gtk_label_get_text(GTK_LABEL(Find(root, "media.storyboard.message")))) == 0);
            /* Retire again after task completion: no stale borrowed token. */
            UmiMediaStoryboardGtkRetire(root);
            g_free(message);
        }
    } else if (strcmp(name, "disabled-save") == 0 || strcmp(name, "disabled-open") == 0) {
        bool opening = strcmp(name, "disabled-open") == 0;
        if (opening) WriteIncoming(path, false);
        Approve(root, opening);
        gtk_widget_set_sensitive(root, FALSE);
        g_signal_emit_by_name(Find(root, opening ? "media.storyboard.file.open" : "media.storyboard.file.save"), "clicked");
        CHECK(!UmiMediaStoryboardGtkBusy(root) && strcmp(Title(root), "Current") == 0);
        if (!opening) CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
        gtk_widget_set_sensitive(root, TRUE);
    } else if (strcmp(name, "save-failure") == 0) {
        SetEntry(root, "media.storyboard.file.path", directory);
        Approve(root, false); Click(root, "media.storyboard.file.save"); Wait(root);
        CHECK(strcmp(Title(root), "Current") == 0 && SceneCount(root) == 1U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "media.storyboard.message"))), "failed") != NULL);
    } else CHECK(false);

    UmiMediaStoryboardGtkRetire(root);
    Wait(root);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(root); g_object_unref(window); Pump(50U);
    if (g_file_test(path, G_FILE_TEST_EXISTS)) CHECK(g_remove(path) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(path); g_free(directory);
    return 0;
}
