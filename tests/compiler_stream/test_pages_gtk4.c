/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_pages_gtk4.c
 * PURPOSE: Check diagnostic page navigation, display stability and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_diagnostics_gtk4.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
static void Seed(UmiBuildDiagnosticPage *page, uint64_t operation, size_t phase, size_t first)
{
    memset(page, 0, sizeof *page);
    page->progress.operation_id = operation;
    page->progress.phase = phase == 0U ? UMI_BUILD_PHASE_CONFIGURE : UMI_BUILD_PHASE_BUILD;
    page->progress.phase_index = phase;
    page->progress.streamed = true;
    page->first_index = first;
    page->retained_count = 19U;
    page->count = first == 0U ? 16U : 3U;
    for (size_t index = 0U; index < page->count; ++index)
    {
        UmiBuildDiagnostic *item = &page->items[index];
        strcpy(item->file, "src/example.c");
        item->line = first + index + 1U;
        item->column = 2U;
        item->severity = UMI_BUILD_DIAGNOSTIC_ERROR;
        (void)snprintf(item->message, sizeof item->message, "record %zu", first + index);
    }
}
static char *Text(GtkTextBuffer *buffer)
{
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
}
static void Reenter(GtkTextBuffer *buffer, gpointer context)
{
    (void)buffer;
    /* A synchronous text notification must not advance a half-rendered page. */
    g_signal_emit_by_name(context, "clicked");
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    GtkWidget *panel = UmiBuildDiagnosticViewGtk4Create();
    g_object_ref_sink(panel);
    GtkWidget *view = umi_gtk4_automation_find_tagged_widget(panel, "build.diagnostics.text");
    GtkWidget *summary = umi_gtk4_automation_find_tagged_widget(panel, "build.diagnostics.summary");
    GtkWidget *previous =
        umi_gtk4_automation_find_tagged_widget(panel, "build.diagnostics.previous");
    GtkWidget *next = umi_gtk4_automation_find_tagged_widget(panel, "build.diagnostics.next");
    CHECK(GTK_IS_TEXT_VIEW(view) && GTK_IS_LABEL(summary) && GTK_IS_BUTTON(previous) &&
          GTK_IS_BUTTON(next));
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(view)));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    UmiBuildDiagnosticPage *page = g_new0(UmiBuildDiagnosticPage, 1U);
    Seed(page, 1U, 0U, 0U);
    CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
    uint64_t operation = 0U;
    size_t phase = 0U, first = 0U;
    CHECK(UmiBuildDiagnosticViewGtk4Request(panel, &operation, &phase, &first) == UMI_STATUS_OK);
    CHECK(operation == 1U && first == 0U && phase == 0U);
    char *initial = Text(buffer);
    CHECK(strstr(initial, "example.c:1:2: error") != NULL && strstr(initial, "record 15") != NULL);
    const char *mode = argv[1];
    if (strcmp(mode, "paging") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(previous) && gtk_widget_get_sensitive(next));
        g_signal_emit_by_name(next, "clicked");
        CHECK(UmiBuildDiagnosticViewGtk4Request(panel, &operation, &phase, &first) ==
                  UMI_STATUS_OK &&
              first == 16U);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_INVALID_STATE);
        Seed(page, 1U, 0U, 16U);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        CHECK(gtk_widget_get_sensitive(previous) && !gtk_widget_get_sensitive(next));
        char *last = Text(buffer);
        CHECK(strstr(last, "record 18") != NULL && strstr(last, "record 0\n") == NULL);
        g_free(last);
        g_signal_emit_by_name(previous, "clicked");
        CHECK(UmiBuildDiagnosticViewGtk4Request(panel, &operation, &phase, &first) ==
                  UMI_STATUS_OK &&
              first == 0U);
    }
    else if (strcmp(mode, "phase") == 0 || strcmp(mode, "operation") == 0)
    {
        g_signal_emit_by_name(next, "clicked");
        Seed(page, strcmp(mode, "operation") == 0 ? 2U : 1U, 1U, 0U);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        CHECK(UmiBuildDiagnosticViewGtk4Request(panel, &operation, &phase, &first) ==
              UMI_STATUS_OK);
        CHECK(first == 0U && phase == 1U);
        Seed(page, 1U, 0U, 0U);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_INVALID_STATE);
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        page->count = 1U;
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_INVALID_ARGUMENT);
        Seed(page, 1U, 0U, 0U);
        memset(page->items[0].file, 'x', sizeof page->items[0].file);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_INVALID_ARGUMENT);
        char *unchanged = Text(buffer);
        CHECK(strcmp(initial, unchanged) == 0);
        g_free(unchanged);
        CHECK(UmiBuildDiagnosticViewGtk4Request(NULL, &operation, &phase, &first) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "selection") == 0)
    {
        GtkTextIter begin, end;
        gtk_text_buffer_get_iter_at_offset(buffer, &begin, 0);
        gtk_text_buffer_get_iter_at_offset(buffer, &end, 5);
        gtk_text_buffer_select_range(buffer, &begin, &end);
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        CHECK(gtk_text_buffer_get_selection_bounds(buffer, &begin, &end));
        CHECK(gtk_text_iter_get_offset(&begin) == 0 && gtk_text_iter_get_offset(&end) == 5);
    }
    else if (strcmp(mode, "losses") == 0)
    {
        page->retention_dropped = 17U;
        page->progress.unrepresented = 3U;
        page->progress.phase_complete = true;
        page->progress.streamed = false;
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        const char *label = gtk_label_get_text(GTK_LABEL(summary));
        CHECK(strstr(label, "17 retention losses") != NULL &&
              strstr(label, "3 parser losses") != NULL);
        CHECK(strstr(label, "phase ended; read its process result") != NULL);
        CHECK(strstr(label, "final preview only") != NULL);
    }
    else if (strcmp(mode, "unicode") == 0)
    {
        strcpy(page->items[0].message, "caf\xc3\xa9 <b>literal</b> \xff");
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        char *display = Text(buffer);
        CHECK(g_utf8_validate(display, -1, NULL) && strstr(display, "<b>literal</b>") != NULL);
        g_free(display);
    }
    else if (strcmp(mode, "reentrant") == 0)
    {
        g_signal_connect(buffer, "changed", G_CALLBACK(Reenter), next);
        strcpy(page->items[0].message, "New compiler record");
        CHECK(UmiBuildDiagnosticViewGtk4Update(panel, page) == UMI_STATUS_OK);
        CHECK(UmiBuildDiagnosticViewGtk4Request(panel, &operation, &phase, &first) ==
                  UMI_STATUS_OK &&
              first == 0U);
        g_signal_handlers_disconnect_by_data(buffer, next);
    }
    else if (strcmp(mode, "retained") == 0)
    {
        g_object_ref(next);
        g_object_ref(previous);
        g_object_unref(panel);
        panel = NULL;
        g_signal_emit_by_name(next, "clicked");
        g_signal_emit_by_name(previous, "clicked");
        g_object_unref(next);
        g_object_unref(previous);
    }
    else
        CHECK(0 && "Unknown case");
    g_free(initial);
    g_free(page);
    if (panel != NULL)
        g_object_unref(panel);
    return 0;
}
