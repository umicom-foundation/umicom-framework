/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_source_gtk4.c
 * PURPOSE: Check live diagnostic source selection, identity and callback lifetime in the shared presenter.
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
typedef struct Probe
{
    GtkWidget *panel;
    unsigned calls, destroyed;
    uint64_t operation;
    size_t phase, index;
    int close;
} Probe;
static UmiStatus Open(uint64_t operation, size_t phase, size_t index, void *context)
{
    Probe *probe = context;
    ++probe->calls;
    probe->operation = operation;
    probe->phase = phase;
    probe->index = index;
    if (probe->close)
    {
        g_object_unref(probe->panel);
        probe->panel = NULL;
    }
    return UMI_STATUS_OK;
}
static void Dispose(gpointer context) { ++((Probe *)context)->destroyed; }
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    if (!gtk_init_check())
        return 77;
    Probe probe = {0};
    probe.panel = g_object_ref_sink(UmiBuildDiagnosticViewGtk4Create());
    GtkWidget *open = umi_gtk4_automation_find_tagged_widget(probe.panel, "build.diagnostics.open");
    GtkWidget *text = umi_gtk4_automation_find_tagged_widget(probe.panel, "build.diagnostics.text");
    GtkWidget *next = umi_gtk4_automation_find_tagged_widget(probe.panel, "build.diagnostics.next");
    CHECK(GTK_IS_BUTTON(open) && GTK_IS_TEXT_VIEW(text) && GTK_IS_BUTTON(next));
    g_object_ref(open);
    CHECK(!gtk_widget_get_sensitive(open));
    CHECK(UmiBuildDiagnosticViewGtk4SetSourceOpener(probe.panel, Open, &probe, Dispose) ==
          UMI_STATUS_OK);
    CHECK(UmiBuildDiagnosticViewGtk4SetSourceOpener(probe.panel, Open, &probe, Dispose) ==
          UMI_STATUS_ALREADY_EXISTS);
    UmiBuildDiagnosticPage *page = g_new0(UmiBuildDiagnosticPage, 1U);
    page->progress.operation_id = 12U;
    page->progress.phase = UMI_BUILD_PHASE_BUILD;
    page->progress.phase_index = 1U;
    page->retained_count = 17U;
    page->count = 16U;
    for (size_t index = 0U; index < page->count; ++index)
    {
        strcpy(page->items[index].file, "caf\xc3\xa9.c");
        page->items[index].line = index + 1U;
        page->items[index].severity = UMI_BUILD_DIAGNOSTIC_ERROR;
        snprintf(page->items[index].message, sizeof page->items[index].message,
                 "message %zu\ncontinuation", index);
    }
    CHECK(UmiBuildDiagnosticViewGtk4Update(probe.panel, page) == UMI_STATUS_OK);
    CHECK(gtk_widget_get_sensitive(open));
    const char *mode = argv[1];
    if (strcmp(mode, "cursor") == 0)
    {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
        GtkTextIter begin, found, end;
        gtk_text_buffer_get_start_iter(buffer, &begin);
        CHECK(gtk_text_iter_forward_search(&begin, "message 3", GTK_TEXT_SEARCH_TEXT_ONLY, &found,
                                           &end, NULL));
        gtk_text_buffer_place_cursor(buffer, &found);
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.calls == 1U && probe.operation == 12U && probe.phase == 1U &&
              probe.index == 3U);
    }
    else if (strcmp(mode, "pending-page") == 0)
    {
        g_signal_emit_by_name(next, "clicked");
        CHECK(!gtk_widget_get_sensitive(open));
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.calls == 0U);
    }
    else if (strcmp(mode, "retained") == 0)
    {
        g_object_unref(probe.panel);
        probe.panel = NULL;
        CHECK(probe.destroyed == 1U);
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.calls == 0U);
    }
    else if (strcmp(mode, "close") == 0)
    {
        probe.close = 1;
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.calls == 1U && probe.destroyed == 1U && probe.panel == NULL);
    }
    else if (strcmp(mode, "stale") == 0)
    {
        page->progress.operation_id = 11U;
        CHECK(UmiBuildDiagnosticViewGtk4Update(probe.panel, page) == UMI_STATUS_INVALID_STATE);
        g_signal_emit_by_name(open, "clicked");
        CHECK(probe.operation == 12U);
    }
    else
        CHECK(0);
    if (probe.panel != NULL)
        g_object_unref(probe.panel);
    CHECK(probe.destroyed == 1U);
    g_object_unref(open);
    g_free(page);
    return 0;
}
