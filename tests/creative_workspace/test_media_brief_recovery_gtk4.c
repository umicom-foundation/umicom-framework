/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_brief_recovery_gtk4.c
 *
 * PURPOSE:
 *   Exercise native creation-document storage, independent edited prompts and protected asynchronous completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/media_brief.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/creative_workspace/media_brief_document.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    GtkWidget *widget = umi_gtk4_automation_find_tagged_widget(root, id);
    CHECK(widget != NULL); return widget;
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
    CHECK(gtk_widget_activate(button)); Pump(300U);
}
static void Entry(GtkWidget *root, const char *id, const char *text)
{ gtk_editable_set_text(GTK_EDITABLE(Find(root, id)), text); }
static void Text(GtkWidget *root, const char *id, const char *text)
{ gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, id))), text, -1); }
static void ExpectText(GtkWidget *root, const char *id, const char *expected)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, id)));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    CHECK(strcmp(text, expected) == 0); g_free(text);
}
static const char *Title(GtkWidget *root)
{ return gtk_editable_get_text(GTK_EDITABLE(Find(root, "media.brief.title"))); }
static void Approve(GtkWidget *root, bool opening)
{ gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, opening ? "media.brief.file.allow-open" : "media.brief.file.allow-save")), TRUE); }
static bool Approved(GtkWidget *root, bool opening)
{ return gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, opening ? "media.brief.file.allow-open" : "media.brief.file.allow-save"))); }
static void Wait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiMediaBriefGtkBusy(root) && g_get_monotonic_time() < end) {
        (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U);
    }
    CHECK(!UmiMediaBriefGtkBusy(root));
}
static UmiMediaBriefDocument Incoming(bool empty)
{
    UmiMediaBriefDocument document = {0};
    if (!empty) {
        document.draft.kind = UMI_MEDIA_BRIEF_MUSIC;
        strcpy(document.draft.title, "From disk");
        strcpy(document.draft.source, "A new scene from a saved document.");
        strcpy(document.draft.direction, "Keep the rhythm gentle.");
        strcpy(document.prompt, "A reviewed prompt that differs from its source.");
    }
    return document;
}
static void WriteIncoming(const char *path, bool empty)
{
    UmiMediaBriefDocument document = Incoming(empty);
    CHECK(UmiMediaBriefDocumentSave(&document, path, NULL) == UMI_STATUS_OK);
}
/* Synchronous semantic activation is reserved for deterministic interleavings:
 * mutate or retire before the main context can dispatch completion. Other cases
 * activate mapped native controls. This is not pointer or visual qualification. */
static void StartOpen(GtkWidget *root)
{
    Approve(root, true);
    g_signal_emit_by_name(Find(root, "media.brief.file.open"), "clicked");
    CHECK(UmiMediaBriefGtkBusy(root));
    CHECK(!Approved(root, true) && !Approved(root, false));
}
static void Unready(GtkWidget *root)
{
    CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.models")));
    CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.image")));
    CHECK(!gtk_widget_get_sensitive(Find(root, "media.brief.video")));
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    char *directory = g_dir_make_tmp("umicom-creation-recovery-XXXXXX", NULL);
    CHECK(directory != NULL);
    char *path = g_build_filename(directory, "creation.json", NULL);
    GtkWidget *window = gtk_window_new(); g_object_ref_sink(window);
    gtk_window_set_default_size(GTK_WINDOW(window), 1300, 1000);
    GtkWidget *root = UmiMediaBriefGtkCreate(NULL, NULL); CHECK(root != NULL);
    g_object_ref_sink(root); gtk_window_set_child(GTK_WINDOW(window), root);
    gtk_window_present(GTK_WINDOW(window)); Pump(300U);
    Entry(root, "media.brief.title", "Current");
    Text(root, "media.brief.source", "Current source.");
    Text(root, "media.brief.direction", "Current direction.");
    Text(root, "media.brief.prompt", "Current edited prompt.");
    GtkWidget *files = Find(root, "media.brief.file");
    CHECK(GTK_IS_EXPANDER(files));
    gtk_expander_set_expanded(GTK_EXPANDER(files), TRUE); Pump(300U);
    Entry(root, "media.brief.file.path", path);
    if (strcmp(name, "save-open") == 0 || strcmp(name, "save-incomplete") == 0 ||
        strcmp(name, "save-stale") == 0 || strcmp(name, "unicode") == 0 ||
        strcmp(name, "prompt-only") == 0) {
        const char *expected_title = "Current", *expected_source = "Current source.";
        const char *expected_prompt = "Current edited prompt.";
        if (strcmp(name, "save-incomplete") == 0 || strcmp(name, "prompt-only") == 0) {
            expected_source = ""; Text(root, "media.brief.source", "");
            if (strcmp(name, "prompt-only") == 0) {
                expected_title = ""; Entry(root, "media.brief.title", "");
                Text(root, "media.brief.direction", "");
            }
        } else if (strcmp(name, "save-stale") == 0) {
            expected_source = "Changed source with the previous edited prompt.";
            Text(root, "media.brief.source", expected_source);
        } else if (strcmp(name, "unicode") == 0) {
            expected_title = "Caf\xc3\xa9";
            expected_prompt = "Dawn \xe2\x98\x80 and sea \xf0\x9f\x8c\x8a.";
            Entry(root, "media.brief.title", expected_title);
            Text(root, "media.brief.prompt", expected_prompt);
        }
        Approve(root, false); Click(root, "media.brief.file.save"); Wait(root);
        CHECK(!Approved(root, false));
        UmiMediaBriefDocument saved = {0};
        CHECK(UmiMediaBriefDocumentLoad(path, NULL, &saved) == UMI_STATUS_OK);
        CHECK(strcmp(saved.draft.title, expected_title) == 0);
        CHECK(strcmp(saved.draft.source, expected_source) == 0);
        CHECK(strcmp(saved.prompt, expected_prompt) == 0);
        Entry(root, "media.brief.title", "Changed after save");
        Text(root, "media.brief.prompt", "Changed after save.");
        Approve(root, true); Click(root, "media.brief.file.open"); Wait(root);
        CHECK(strcmp(Title(root), expected_title) == 0);
        ExpectText(root, "media.brief.source", expected_source);
        ExpectText(root, "media.brief.prompt", expected_prompt);
        Unready(root);
    } else if (strcmp(name, "load-empty") == 0) {
        WriteIncoming(path, true);
        Approve(root, true); Click(root, "media.brief.file.open"); Wait(root);
        CHECK(strcmp(Title(root), "") == 0);
        ExpectText(root, "media.brief.source", "");
        ExpectText(root, "media.brief.prompt", ""); Unready(root);
    } else if (strcmp(name, "save-approval") == 0 || strcmp(name, "open-approval") == 0) {
        bool opening = strcmp(name, "open-approval") == 0;
        if (opening) WriteIncoming(path, false);
        Click(root, opening ? "media.brief.file.open" : "media.brief.file.save");
        CHECK(!UmiMediaBriefGtkBusy(root) && strcmp(Title(root), "Current") == 0);
        if (!opening) CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    } else if (strcmp(name, "path-reset") == 0 || strcmp(name, "source-reset") == 0 ||
               strcmp(name, "prompt-reset") == 0 || strcmp(name, "kind-reset") == 0) {
        Approve(root, false); Approve(root, true);
        if (strcmp(name, "path-reset") == 0) Entry(root, "media.brief.file.path", directory);
        else if (strcmp(name, "source-reset") == 0) Text(root, "media.brief.source", "Later source.");
        else if (strcmp(name, "prompt-reset") == 0) Text(root, "media.brief.prompt", "Later prompt.");
        else gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(root, "media.brief.kind")), UMI_MEDIA_BRIEF_VIDEO);
        CHECK(!Approved(root, false) && !Approved(root, true));
    } else if (strcmp(name, "damaged-open") == 0) {
        CHECK(g_file_set_contents(path, "{", 1, NULL));
        Approve(root, true); Click(root, "media.brief.file.open"); Wait(root);
        CHECK(strcmp(Title(root), "Current") == 0);
        ExpectText(root, "media.brief.prompt", "Current edited prompt.");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "media.brief.message"))), "failed") != NULL);
    } else if (strcmp(name, "retire-open") == 0 || strcmp(name, "stale-source") == 0 ||
               strcmp(name, "stale-prompt") == 0 || strcmp(name, "stale-path") == 0 ||
               strcmp(name, "busy-set-source") == 0 || strcmp(name, "cancel-open") == 0) {
        WriteIncoming(path, false); StartOpen(root);
        if (strcmp(name, "retire-open") == 0) {
            UmiMediaBriefGtkRetire(root);
            char *message = g_strdup(gtk_label_get_text(GTK_LABEL(Find(root, "media.brief.message"))));
            Wait(root);
            CHECK(strcmp(Title(root), "Current") == 0 && !gtk_widget_is_sensitive(root));
            CHECK(strcmp(message, gtk_label_get_text(GTK_LABEL(Find(root, "media.brief.message")))) == 0);
            UmiMediaBriefGtkRetire(root); g_free(message);
        } else if (strcmp(name, "busy-set-source") == 0) {
            UmiMediaBriefDocument document = Incoming(false);
            CHECK(UmiMediaBriefGtkSetSource(root, &document.draft, true) == UMI_STATUS_BUSY);
            CHECK(strcmp(Title(root), "Current") == 0);
            Wait(root); CHECK(strcmp(Title(root), "From disk") == 0); Unready(root);
        } else if (strcmp(name, "cancel-open") == 0) {
            g_signal_emit_by_name(Find(root, "media.brief.file.stop"), "clicked");
            Wait(root);
            /* Cancellation races the bounded read. Either complete document is
             * admissible; a mixture or renewed handoff permission is not. */
            if (strcmp(Title(root), "Current") == 0)
                ExpectText(root, "media.brief.prompt", "Current edited prompt.");
            else {
                CHECK(strcmp(Title(root), "From disk") == 0);
                ExpectText(root, "media.brief.prompt", "A reviewed prompt that differs from its source.");
            }
            Unready(root);
        } else {
            if (strcmp(name, "stale-source") == 0) Text(root, "media.brief.source", "Later direct source.");
            else if (strcmp(name, "stale-prompt") == 0) Text(root, "media.brief.prompt", "Later direct prompt.");
            else Entry(root, "media.brief.file.path", directory);
            Wait(root); CHECK(strcmp(Title(root), "Current") == 0);
            ExpectText(root, "media.brief.prompt", strcmp(name, "stale-prompt") == 0 ? "Later direct prompt." : "Current edited prompt.");
            if (strcmp(name, "stale-source") == 0) ExpectText(root, "media.brief.source", "Later direct source.");
        }
    } else if (strcmp(name, "disabled-save") == 0 || strcmp(name, "disabled-open") == 0) {
        bool opening = strcmp(name, "disabled-open") == 0;
        if (opening) WriteIncoming(path, false);
        Approve(root, opening); gtk_widget_set_sensitive(root, FALSE);
        g_signal_emit_by_name(Find(root, opening ? "media.brief.file.open" : "media.brief.file.save"), "clicked");
        CHECK(!UmiMediaBriefGtkBusy(root) && strcmp(Title(root), "Current") == 0);
        if (!opening) CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
        gtk_widget_set_sensitive(root, TRUE);
    } else if (strcmp(name, "save-failure") == 0) {
        Entry(root, "media.brief.file.path", directory);
        Approve(root, false); Click(root, "media.brief.file.save"); Wait(root);
        CHECK(strcmp(Title(root), "Current") == 0);
        ExpectText(root, "media.brief.prompt", "Current edited prompt.");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "media.brief.message"))), "failed") != NULL);
    } else if (strcmp(name, "oversized-save") == 0) {
        char *long_text = g_malloc0(UMI_MEDIA_BRIEF_PROMPT_CAPACITY + 1U);
        memset(long_text, 'x', UMI_MEDIA_BRIEF_PROMPT_CAPACITY);
        Text(root, "media.brief.prompt", long_text);
        Approve(root, false); Click(root, "media.brief.file.save");
        CHECK(!UmiMediaBriefGtkBusy(root) && !g_file_test(path, G_FILE_TEST_EXISTS));
        ExpectText(root, "media.brief.prompt", long_text); g_free(long_text);
    } else if (strcmp(name, "prepare-protection") == 0) {
        WriteIncoming(path, false);
        Approve(root, true); Click(root, "media.brief.file.open"); Wait(root);
        Click(root, "media.brief.prepare");
        ExpectText(root, "media.brief.prompt", "A reviewed prompt that differs from its source.");
        Unready(root);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "media.brief.replace")), TRUE);
        Click(root, "media.brief.prepare");
        CHECK(gtk_widget_get_sensitive(Find(root, "media.brief.models")));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(Find(root, "media.brief.replace"))));
    } else CHECK(false);
    UmiMediaBriefGtkRetire(root); Wait(root);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(root); g_object_unref(window); Pump(50U);
    if (g_file_test(path, G_FILE_TEST_EXISTS)) CHECK(g_remove(path) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(path); g_free(directory);
    return 0;
}
