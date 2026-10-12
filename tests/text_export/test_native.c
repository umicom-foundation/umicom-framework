/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_export/test_native.c
 * PURPOSE: Exercise immutable report export and retained native controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #c); result = 1; goto cleanup; } } while (0)

static GtkWidget *Find(GtkWidget *root, const char *id)
{ return umi_gtk4_automation_find_tagged_widget(root, id); }
static bool Wait(GtkWidget *root)
{
    gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (UmiTextExportGtk4Busy(root) && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiTextExportGtk4Busy(root);
}

int main(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    char *directory = g_dir_make_tmp("umicom-text-export-XXXXXX", NULL);
    char *path = directory ? g_build_filename(directory, "caf\xc3\xa9 report.txt", NULL) : NULL;
    char *other = directory ? g_build_filename(directory, "other.txt", NULL) : NULL;
    char *labelPath = directory ? g_build_filename(directory, "label.txt", NULL) : NULL;
    char *gonePath = directory ? g_build_filename(directory, "gone.txt", NULL) : NULL;
    char *content = NULL, *large = NULL;
    GtkWidget *view = gtk_text_view_new(); g_object_ref_sink(view);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkWidget *root = UmiTextExportGtk4Create(view, "report.txt"); g_object_ref_sink(root);
    GtkWidget *button = g_object_ref(Find(root, "text.export.save"));
    GtkWidget *entry = Find(root, "text.export.path");
    GtkWidget *label = gtk_label_new("Manual forecast GBP: 125.00"); g_object_ref_sink(label);
    GtkWidget *labelRoot = UmiTextExportGtk4Create(label, "forecast.txt"); g_object_ref_sink(labelRoot);
    GtkTextBuffer *detached = gtk_text_buffer_new(NULL);
    GtkWidget *bufferRoot = UmiTextExportGtk4CreateForBuffer(detached, "buffer.txt"); g_object_ref_sink(bufferRoot);
    CHECK(directory && path && other && labelPath && gonePath && entry && button);
    CHECK(UmiTextExportGtk4Create(NULL, "invalid.txt") == NULL);
    gtk_text_buffer_set_text(buffer, "Original caf\xc3\xa9 report", -1);
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
    g_signal_emit_by_name(button, "clicked");
    /* A second click and later UI changes cannot alter the accepted path or text. */
    CHECK(UmiTextExportGtk4Busy(root));
    gtk_text_buffer_set_text(buffer, "Later report", -1);
    gtk_editable_set_text(GTK_EDITABLE(entry), other);
    g_signal_emit_by_name(button, "clicked");
    CHECK(Wait(root));
    CHECK(g_file_get_contents(path, &content, NULL, NULL));
    CHECK(strcmp(content, "Original caf\xc3\xa9 report") == 0); g_clear_pointer(&content, g_free);
    CHECK(!g_file_test(other, G_FILE_TEST_EXISTS));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "text.export.status"))), path));
    /* Existing evidence stays byte-for-byte intact. */
    gtk_editable_set_text(GTK_EDITABLE(entry), path); g_signal_emit_by_name(button, "clicked");
    CHECK(Wait(root)); CHECK(g_file_get_contents(path, &content, NULL, NULL));
    CHECK(strcmp(content, "Original caf\xc3\xa9 report") == 0); g_clear_pointer(&content, g_free);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "text.export.status"))), "failed"));
    gtk_editable_set_text(GTK_EDITABLE(entry), other);
    gtk_text_buffer_set_text(buffer, "", -1); g_signal_emit_by_name(button, "clicked");
    CHECK(!UmiTextExportGtk4Busy(root) && !g_file_test(other, G_FILE_TEST_EXISTS));
    /* Reject large reports before opening a file; do not silently truncate. */
    large = g_malloc((size_t)UMI_TEXT_EXPORT_MAX_CHARACTERS + 2U);
    memset(large, 'x', (size_t)UMI_TEXT_EXPORT_MAX_CHARACTERS + 1U);
    large[UMI_TEXT_EXPORT_MAX_CHARACTERS + 1] = '\0';
    gtk_text_buffer_set_text(buffer, large, -1); g_signal_emit_by_name(button, "clicked");
    CHECK(!UmiTextExportGtk4Busy(root) && !g_file_test(other, G_FILE_TEST_EXISTS));
    g_clear_pointer(&large, g_free);
    gtk_text_buffer_set_text(buffer, "A report accepted before closing", -1);
    g_signal_emit_by_name(button, "clicked");
    UmiTextExportGtk4Retire(root); g_object_unref(view); view = NULL;
    CHECK(Wait(root)); CHECK(g_file_get_contents(other, &content, NULL, NULL));
    CHECK(strcmp(content, "A report accepted before closing") == 0); g_clear_pointer(&content, g_free);
    gtk_editable_set_text(GTK_EDITABLE(entry), gonePath); g_signal_emit_by_name(button, "clicked");
    CHECK(!UmiTextExportGtk4Busy(root) && !g_file_test(gonePath, G_FILE_TEST_EXISTS));
    /* Labels use rendered text. Standalone buffers are weakly observed too. */
    gtk_editable_set_text(GTK_EDITABLE(Find(labelRoot, "text.export.path")), labelPath);
    g_signal_emit_by_name(Find(labelRoot, "text.export.save"), "clicked"); CHECK(Wait(labelRoot));
    CHECK(g_file_get_contents(labelPath, &content, NULL, NULL));
    CHECK(strcmp(content, "Manual forecast GBP: 125.00") == 0); g_clear_pointer(&content, g_free);
    gtk_editable_set_text(GTK_EDITABLE(Find(bufferRoot, "text.export.path")), gonePath);
    /* Hidden ranges are not part of displayed text and must not escape through
     * an export of a standalone report buffer. */
    gtk_text_buffer_set_text(detached, "Visible hidden", -1);
    GtkTextIter hideStart, hideEnd;
    gtk_text_buffer_get_iter_at_offset(detached, &hideStart, 8);
    gtk_text_buffer_get_end_iter(detached, &hideEnd);
    GtkTextTag *hidden = gtk_text_buffer_create_tag(detached, "private-range", "invisible", TRUE, NULL);
    gtk_text_buffer_apply_tag(detached, hidden, &hideStart, &hideEnd);
    g_signal_emit_by_name(Find(bufferRoot, "text.export.save"), "clicked"); CHECK(Wait(bufferRoot));
    CHECK(g_file_get_contents(gonePath, &content, NULL, NULL));
    CHECK(strcmp(content, "Visible ") == 0); g_clear_pointer(&content, g_free);
    CHECK(g_remove(gonePath) == 0);
    g_clear_object(&detached);
    g_signal_emit_by_name(Find(bufferRoot, "text.export.save"), "clicked");
    CHECK(!UmiTextExportGtk4Busy(bufferRoot) && !g_file_test(gonePath, G_FILE_TEST_EXISTS));
cleanup:
    UmiTextExportGtk4Retire(root); UmiTextExportGtk4Retire(labelRoot); UmiTextExportGtk4Retire(bufferRoot);
    bool finished = Wait(root) && Wait(labelRoot) && Wait(bufferRoot);
    if (!finished) { fprintf(stderr, "Export worker did not retire.\n"); result = 1; }
    g_clear_object(&view); g_clear_object(&detached);
    g_object_unref(button); g_object_unref(root); g_object_unref(labelRoot); g_object_unref(bufferRoot); g_object_unref(label);
    g_free(content); g_free(large);
    if (finished) {
        if (path) (void)g_remove(path);
        if (other) (void)g_remove(other);
        if (labelPath) (void)g_remove(labelPath);
        if (gonePath) (void)g_remove(gonePath);
        if (directory) (void)g_rmdir(directory);
    }
    g_free(path); g_free(other); g_free(labelPath); g_free(gonePath); g_free(directory);
    return result;
}
