/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/education/test_inspection_gtk4.c
 * PURPOSE: Keep native teaching reports tied to explicit files and live widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/education_inspection.h"
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
    while (UmiEducationInspectionGtkBusy(root) && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiEducationInspectionGtkBusy(root);
}
static char *Report(GtkWidget *root)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(root, "education.inspection.report")));
    GtkTextIter begin, end; gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    char *directory = g_dir_make_tmp("umicom-inspection-ui-XXXXXX", NULL);
    char *path = directory ? g_build_filename(directory, "caf\xc3\xa9 teaching.trace", NULL) : NULL;
    GtkWidget *root = UmiEducationInspectionGtkCreate(); g_object_ref_sink(root);
    GtkWidget *button = g_object_ref(Find(root, "education.inspection.read"));
    char *before = NULL, *after = NULL;
    char *missing = directory ? g_build_filename(directory, "not-present.trace", NULL) : NULL;
    CHECK(directory && path && button);
    GtkWidget *entry = Find(root, "education.inspection.path");
    GtkWidget *format = Find(root, "education.inspection.format");
    const char *magic[] = {"UMISCH1", "UMITMR1", "UMIIRQ1", "UMIEVD1", "UMIDIO1"};
    const char *headings[] = {"Scheduler records: 0", "Timer records: 0", "Interrupt records: 0", "Driver evidence records: 0", "Device I/O records: 0"};
    /* Real, minimal empty traces exercise each public parser and the native
     * Unicode file reader. A format choice is required; no executable runs. */
    for (guint index = 0U; index < G_N_ELEMENTS(magic); ++index) {
        unsigned char header[24] = {0}; memcpy(header, magic[index], 7U); header[8] = 1U;
        CHECK(g_file_set_contents(path, (const char *)header, index >= 3U ? 24 : 16, NULL));
        gtk_editable_set_text(GTK_EDITABLE(entry), path); gtk_drop_down_set_selected(GTK_DROP_DOWN(format), index);
        g_signal_emit_by_name(button, "clicked"); CHECK(Wait(root));
        after = Report(root); CHECK(strstr(after, headings[index]) && strstr(after, path)); g_clear_pointer(&after, g_free);
    }
    before = Report(root);
    /* Mismatched formats and missing files preserve the labelled last report. */
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
    g_signal_emit_by_name(button, "clicked"); CHECK(Wait(root));
    after = Report(root); CHECK(strcmp(before, after) == 0); g_clear_pointer(&after, g_free);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "education.inspection.status"))), "failed"));
    gtk_editable_set_text(GTK_EDITABLE(entry), missing);
    g_signal_emit_by_name(button, "clicked"); CHECK(Wait(root));
    after = Report(root); CHECK(strcmp(before, after) == 0); g_clear_pointer(&after, g_free);
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 4U);
    g_signal_emit_by_name(button, "clicked");
    gtk_editable_set_text(GTK_EDITABLE(entry), directory);
    CHECK(Wait(root)); after = Report(root); CHECK(strcmp(before, after) == 0); g_clear_pointer(&after, g_free);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(root, "education.inspection.status"))), "discarded"));
    gtk_editable_set_text(GTK_EDITABLE(entry), path); g_signal_emit_by_name(button, "clicked");
    g_signal_emit_by_name(Find(root, "education.inspection.discard"), "clicked");
    CHECK(Wait(root)); after = Report(root); CHECK(strcmp(before, after) == 0); g_clear_pointer(&after, g_free);
    /* Retirement makes retained controls inert while an accepted read finishes. */
    g_signal_emit_by_name(button, "clicked"); UmiEducationInspectionGtkRetire(root);
    CHECK(Wait(root)); g_signal_emit_by_name(button, "clicked"); CHECK(!UmiEducationInspectionGtkBusy(root));
    CHECK(!gtk_widget_get_sensitive(Find(root, "text.export")));
    after = Report(root); CHECK(strcmp(before, after) == 0);
cleanup:
    UmiEducationInspectionGtkRetire(root);
    bool finished = Wait(root);
    if (!finished) { fprintf(stderr, "Inspection worker did not retire.\n"); result = 1; }
    g_object_unref(root); g_object_unref(button); g_free(before); g_free(after);
    if (finished && path) (void)g_remove(path);
    if (finished && directory) (void)g_rmdir(directory);
    g_free(path); g_free(missing); g_free(directory); return result;
}

/* The original four-format fixture selections remain for review. The active
 * case above preserves those fixtures and extends the same assertions to the
 * new device I/O format, whose header also occupies 24 bytes. */
#if 0
    const char *magic[] = {"UMISCH1", "UMITMR1", "UMIIRQ1", "UMIEVD1"};
    const char *headings[] = {"Scheduler records: 0", "Timer records: 0", "Interrupt records: 0", "Driver evidence records: 0"};
    CHECK(g_file_set_contents(path, (const char *)header, index == 3U ? 24 : 16, NULL));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 3U);
#endif
