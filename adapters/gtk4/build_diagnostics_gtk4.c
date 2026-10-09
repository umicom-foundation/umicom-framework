/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/build_diagnostics_gtk4.c
 * PURPOSE: Keep live compiler paging and display ownership in the shared GTK adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_diagnostics_gtk4.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>

typedef struct BuildDiagnosticView
{
    GtkWidget *summary, *text, *previous, *next;
    uint64_t operation;
    size_t phase, first, requested, count, retained;
    int changing;
    char *rendered_text;
    GtkWidget *open_source;
    UmiBuildDiagnosticSourceOpen opener;
    void *open_context;
    GDestroyNotify destroy_context;
    gint record_offsets[UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY];
} BuildDiagnosticView;
/* The optional source opener owns a host binding until the diagnostic panel is released. The original state destructor remains for review. The previous implementation is retained for engineering review. */
#if 0
static void DiagnosticViewFree(gpointer data)
{
    BuildDiagnosticView *view = data;
    g_free(view->rendered_text);
    g_free(view);
}
#endif
static void DiagnosticViewFree(gpointer data)
{
    BuildDiagnosticView *view = data;
    if (view->destroy_context != NULL) view->destroy_context(view->open_context);
    g_free(view->rendered_text);
    g_free(view);
}
static BuildDiagnosticView *DiagnosticViewState(GtkWidget *panel)
{
    return panel != NULL && GTK_IS_BOX(panel)
               ? g_object_get_data(G_OBJECT(panel), "umicom-build-diagnostic-view")
               : NULL;
}
/* Source actions wait until the requested page has arrived. The previous page-navigation implementation is retained for review. The previous implementation is retained for engineering review. */
#if 0
static void DiagnosticViewMove(GtkButton *button, gpointer context)
{
    GtkWidget *panel = context;
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || view->changing)
        return;
    g_object_ref(panel);
    view->changing = 1;
    if (GTK_WIDGET(button) == view->previous && view->first != 0U)
        view->requested = view->first >= UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                              ? view->first - UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                              : 0U;
    else if (GTK_WIDGET(button) == view->next && view->count < view->retained - view->first)
        view->requested = view->first + view->count;
    gtk_label_set_text(GTK_LABEL(view->summary), "Reading the requested diagnostic page...");
    gtk_widget_set_sensitive(view->previous, FALSE);
    gtk_widget_set_sensitive(view->next, FALSE);
    view->changing = 0;
    g_object_unref(panel);
}
#endif
static void DiagnosticViewMove(GtkButton *button, gpointer context)
{
    GtkWidget *panel = context;
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || view->changing)
        return;
    g_object_ref(panel);
    view->changing = 1;
    if (GTK_WIDGET(button) == view->previous && view->first != 0U)
        view->requested = view->first >= UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                              ? view->first - UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                              : 0U;
    else if (GTK_WIDGET(button) == view->next && view->count < view->retained - view->first)
        view->requested = view->first + view->count;
    gtk_label_set_text(GTK_LABEL(view->summary), "Reading the requested diagnostic page...");
    gtk_widget_set_sensitive(view->previous, FALSE);
    gtk_widget_set_sensitive(view->next, FALSE);
    gtk_widget_set_sensitive(view->open_source, FALSE);
    view->changing = 0;
    g_object_unref(panel);
}

/* The insertion cursor chooses a record without parsing displayed text back
 * into a path. Identity is revalidated by the host against the live producer. */
static void DiagnosticViewOpen(GtkButton *button, gpointer context)
{
    (void)button;
    GtkWidget *panel = context;
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || view->changing || view->opener == NULL ||
        view->count == 0U || view->requested != view->first) return;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view->text));
    GtkTextIter cursor;
    gtk_text_buffer_get_iter_at_mark(buffer, &cursor, gtk_text_buffer_get_insert(buffer));
    gint offset = gtk_text_iter_get_offset(&cursor);
    size_t row = 0U;
    for (size_t index = 1U; index < view->count; ++index)
        if (offset >= view->record_offsets[index]) row = index;
    g_object_ref(panel);
    view->changing = 1;
    UmiStatus status = view->opener(view->operation, view->phase, view->first + row, view->open_context);
    if (status != UMI_STATUS_OK)
        gtk_label_set_text(GTK_LABEL(view->summary),
            status == UMI_STATUS_INVALID_STATE ? "This diagnostic is stale or its source path is ambiguous. Refresh and review the message."
            : "The source location could not be opened. Review the path and line in the message.");
    view->changing = 0;
    g_object_unref(panel);
}
UmiStatus UmiBuildDiagnosticViewGtk4SetSourceOpener(GtkWidget *panel,
    UmiBuildDiagnosticSourceOpen opener, void *context, GDestroyNotify destroy_context)
{
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || opener == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->changing) return UMI_STATUS_BUSY;
    if (view->opener != NULL) return UMI_STATUS_ALREADY_EXISTS;
    view->opener = opener;
    view->open_context = context;
    view->destroy_context = destroy_context;
    gtk_widget_set_sensitive(view->open_source, view->count != 0U && view->first == view->requested);
    return UMI_STATUS_OK;
}

/* An explicit source action complements passive diagnostic text and paging. The previous control composition remains for review. The previous implementation is retained for engineering review. */
#if 0
GtkWidget *UmiBuildDiagnosticViewGtk4Create(void)
{
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    BuildDiagnosticView *view = g_new0(BuildDiagnosticView, 1U);
    g_object_set_data_full(G_OBJECT(panel), "umicom-build-diagnostic-view", view,
                           DiagnosticViewFree);
    view->summary = gtk_label_new("No running-build diagnostics yet.");
    gtk_label_set_wrap(GTK_LABEL(view->summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(view->summary), 0.0F);
    gtk_box_append(GTK_BOX(panel), view->summary);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    view->previous = gtk_button_new_with_label("Previous diagnostics");
    view->next = gtk_button_new_with_label("Next diagnostics");
    gtk_box_append(GTK_BOX(buttons), view->previous);
    gtk_box_append(GTK_BOX(buttons), view->next);
    gtk_box_append(GTK_BOX(panel), buttons);
    gtk_widget_set_sensitive(view->previous, FALSE);
    gtk_widget_set_sensitive(view->next, FALSE);
    view->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->text), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view->text);
    gtk_box_append(GTK_BOX(panel), scroll);
    (void)umi_gtk4_automation_tag_widget(panel, "build.diagnostics.panel");
    (void)umi_gtk4_automation_tag_widget(view->summary, "build.diagnostics.summary");
    (void)umi_gtk4_automation_tag_widget(view->text, "build.diagnostics.text");
    (void)umi_gtk4_automation_tag_widget(view->previous, "build.diagnostics.previous");
    (void)umi_gtk4_automation_tag_widget(view->next, "build.diagnostics.next");
    /* Object-bound signals disconnect when the panel dies, even if a host
     * retained one button. No callback then dereferences the released state. */
    g_signal_connect_object(view->previous, "clicked", G_CALLBACK(DiagnosticViewMove), panel, 0);
    g_signal_connect_object(view->next, "clicked", G_CALLBACK(DiagnosticViewMove), panel, 0);
    return panel;
}
#endif
GtkWidget *UmiBuildDiagnosticViewGtk4Create(void)
{
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    BuildDiagnosticView *view = g_new0(BuildDiagnosticView, 1U);
    g_object_set_data_full(G_OBJECT(panel), "umicom-build-diagnostic-view", view,
                           DiagnosticViewFree);
    view->summary = gtk_label_new("No running-build diagnostics yet.");
    gtk_label_set_wrap(GTK_LABEL(view->summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(view->summary), 0.0F);
    gtk_box_append(GTK_BOX(panel), view->summary);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    view->previous = gtk_button_new_with_label("Previous diagnostics");
    view->next = gtk_button_new_with_label("Next diagnostics");
    view->open_source = gtk_button_new_with_label("Open source at cursor");
    gtk_widget_set_tooltip_text(view->open_source, "Click within a compiler message, then open its current source location.");
    gtk_widget_set_sensitive(view->open_source, FALSE);
    (void)umi_gtk4_automation_tag_widget(view->open_source, "build.diagnostics.open");
    g_signal_connect_object(view->open_source, "clicked", G_CALLBACK(DiagnosticViewOpen), panel, 0);
    gtk_box_append(GTK_BOX(buttons), view->previous);
    gtk_box_append(GTK_BOX(buttons), view->next);
    gtk_box_append(GTK_BOX(buttons), view->open_source);
    gtk_box_append(GTK_BOX(panel), buttons);
    gtk_widget_set_sensitive(view->previous, FALSE);
    gtk_widget_set_sensitive(view->next, FALSE);
    view->text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view->text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view->text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view->text), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view->text);
    gtk_box_append(GTK_BOX(panel), scroll);
    (void)umi_gtk4_automation_tag_widget(panel, "build.diagnostics.panel");
    (void)umi_gtk4_automation_tag_widget(view->summary, "build.diagnostics.summary");
    (void)umi_gtk4_automation_tag_widget(view->text, "build.diagnostics.text");
    (void)umi_gtk4_automation_tag_widget(view->previous, "build.diagnostics.previous");
    (void)umi_gtk4_automation_tag_widget(view->next, "build.diagnostics.next");
    /* Object-bound signals disconnect when the panel dies, even if a host
     * retained one button. No callback then dereferences the released state. */
    g_signal_connect_object(view->previous, "clicked", G_CALLBACK(DiagnosticViewMove), panel, 0);
    g_signal_connect_object(view->next, "clicked", G_CALLBACK(DiagnosticViewMove), panel, 0);
    return panel;
}
UmiStatus UmiBuildDiagnosticViewGtk4Request(GtkWidget *panel, uint64_t *operation,
                                            size_t *phase_index, size_t *first_index)
{
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || operation == NULL || phase_index == NULL || first_index == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *operation = view->operation;
    *phase_index = view->phase;
    *first_index = view->requested;
    return UMI_STATUS_OK;
}
/* The captured build path receives the same bounded-string validation as the source path. The previous page check remains for review. The previous implementation is retained for engineering review. */
#if 0
static int DiagnosticPageValid(const UmiBuildDiagnosticPage *page)
{
    if (page == NULL || page->retained_count > UMI_BUILD_MAX_DIAGNOSTICS ||
        page->first_index > page->retained_count ||
        page->count > UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY ||
        page->count > page->retained_count - page->first_index ||
        page->progress.phase_index >= UMI_BUILD_PROJECT_SESSION_MAX_PHASES ||
        page->progress.phase < UMI_BUILD_PHASE_CONFIGURE ||
        page->progress.phase > UMI_BUILD_PHASE_DEPLOY ||
        memchr(page->source_directory, '\0', sizeof page->source_directory) == NULL ||
        (page->progress.operation_id == 0U && (page->count != 0U || page->retained_count != 0U)))
        return 0;
    size_t remaining = page->retained_count - page->first_index;
    size_t expected = remaining < UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                          ? remaining
                          : UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY;
    if (page->count != expected)
        return 0;
    for (size_t index = 0U; index < page->count; ++index)
    {
        const UmiBuildDiagnostic *item = &page->items[index];
        if (memchr(item->file, '\0', sizeof item->file) == NULL ||
            memchr(item->code, '\0', sizeof item->code) == NULL ||
            memchr(item->message, '\0', sizeof item->message) == NULL ||
            item->severity < UMI_BUILD_DIAGNOSTIC_NOTE ||
            item->severity > UMI_BUILD_DIAGNOSTIC_FATAL)
            return 0;
    }
    return 1;
}
#endif
static int DiagnosticPageValid(const UmiBuildDiagnosticPage *page)
{
    if (page == NULL || page->retained_count > UMI_BUILD_MAX_DIAGNOSTICS ||
        page->first_index > page->retained_count ||
        page->count > UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY ||
        page->count > page->retained_count - page->first_index ||
        page->progress.phase_index >= UMI_BUILD_PROJECT_SESSION_MAX_PHASES ||
        page->progress.phase < UMI_BUILD_PHASE_CONFIGURE ||
        page->progress.phase > UMI_BUILD_PHASE_DEPLOY ||
        memchr(page->source_directory, '\0', sizeof page->source_directory) == NULL ||
        memchr(page->build_directory, '\0', sizeof page->build_directory) == NULL ||
        (page->progress.operation_id == 0U && (page->count != 0U || page->retained_count != 0U)))
        return 0;
    size_t remaining = page->retained_count - page->first_index;
    size_t expected = remaining < UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY
                          ? remaining
                          : UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY;
    if (page->count != expected)
        return 0;
    for (size_t index = 0U; index < page->count; ++index)
    {
        const UmiBuildDiagnostic *item = &page->items[index];
        if (memchr(item->file, '\0', sizeof item->file) == NULL ||
            memchr(item->code, '\0', sizeof item->code) == NULL ||
            memchr(item->message, '\0', sizeof item->message) == NULL ||
            item->severity < UMI_BUILD_DIAGNOSTIC_NOTE ||
            item->severity > UMI_BUILD_DIAGNOSTIC_FATAL)
            return 0;
    }
    return 1;
}
/* Copied character offsets associate cursor positions with producer records without interpreting displayed text. The original passive renderer remains for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus UmiBuildDiagnosticViewGtk4Update(GtkWidget *panel, const UmiBuildDiagnosticPage *page)
{
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || !DiagnosticPageValid(page))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (view->changing)
        return UMI_STATUS_BUSY;
    uint64_t operation = page->progress.operation_id;
    size_t phase = page->progress.phase_index;
    int same = operation == view->operation && phase == view->phase;
    if (operation < view->operation || (operation == view->operation && phase < view->phase) ||
        (same && page->first_index != view->requested) || (!same && page->first_index != 0U))
        return UMI_STATUS_INVALID_STATE;
    GString *text = g_string_new(NULL);
    for (size_t index = 0U; index < page->count; ++index)
    {
        const UmiBuildDiagnostic *item = &page->items[index];
        const char *severity = item->severity == UMI_BUILD_DIAGNOSTIC_FATAL     ? "fatal"
                               : item->severity == UMI_BUILD_DIAGNOSTIC_ERROR   ? "error"
                               : item->severity == UMI_BUILD_DIAGNOSTIC_WARNING ? "warning"
                                                                                : "note";
        g_string_append_printf(text, "%s:%zu:%zu: %s%s%s\n%s\n\n",
                               item->file[0] != '\0' ? item->file : "(no source file)", item->line,
                               item->column, severity, item->code[0] != '\0' ? " " : "", item->code,
                               item->message);
    }
    if (page->count == 0U)
        g_string_append(text, "No retained diagnostics on this page.");
    char *display = g_utf8_make_valid(text->str, (gssize)text->len);
    g_string_free(text, TRUE);
    char *summary =
        operation == 0U
            ? g_strdup("No running-build diagnostics yet.")
            : g_strdup_printf("%s | %s | records %zu–%zu of %zu | %" G_GUINT64_FORMAT
                              " retention losses | %" G_GUINT64_FORMAT " parser losses%s",
                              umi_build_phase_text(page->progress.phase),
                              page->progress.phase_complete ? "phase ended; read its process result"
                                                            : "running",
                              page->count != 0U ? page->first_index + 1U : page->first_index,
                              page->first_index + page->count, page->retained_count,
                              (guint64)page->retention_dropped,
                              (guint64)page->progress.unrepresented,
                              page->progress.streamed ? "" : " | final preview only");
    /* GTK property notifications can re-enter host code. Retain the panel and
     * fence navigation until the complete copied page is visible. */
    g_object_ref(panel);
    view->changing = 1;
    view->operation = operation;
    view->phase = phase;
    view->first = page->first_index;
    view->requested = page->first_index;
    view->count = page->count;
    view->retained = page->retained_count;
    /* Repeated polling must preserve the user's selection and scroll position
     * when no record on this page changed. */
    if (view->rendered_text == NULL || strcmp(view->rendered_text, display) != 0)
    {
        g_free(view->rendered_text);
        view->rendered_text = g_strdup(display);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view->text)), display, -1);
    }
    gtk_label_set_text(GTK_LABEL(view->summary), summary);
    gtk_widget_set_sensitive(view->previous, view->first != 0U);
    gtk_widget_set_sensitive(view->next, view->count < view->retained - view->first);
    view->changing = 0;
    g_free(display);
    g_free(summary);
    g_object_unref(panel);
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiBuildDiagnosticViewGtk4Update(GtkWidget *panel, const UmiBuildDiagnosticPage *page)
{
    BuildDiagnosticView *view = DiagnosticViewState(panel);
    if (view == NULL || !DiagnosticPageValid(page))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (view->changing)
        return UMI_STATUS_BUSY;
    uint64_t operation = page->progress.operation_id;
    size_t phase = page->progress.phase_index;
    int same = operation == view->operation && phase == view->phase;
    if (operation < view->operation || (operation == view->operation && phase < view->phase) ||
        (same && page->first_index != view->requested) || (!same && page->first_index != 0U))
        return UMI_STATUS_INVALID_STATE;
    GString *text = g_string_new(NULL);
    gint offsets[UMI_BUILD_DIAGNOSTIC_PAGE_CAPACITY] = {0};
    for (size_t index = 0U; index < page->count; ++index)
    {
        /* GTK offsets count characters, not raw compiler bytes. Repair the
         * prefix exactly as the renderer does before recording this boundary. */
        char *prefix = g_utf8_make_valid(text->str, (gssize)text->len);
        offsets[index] = (gint)g_utf8_strlen(prefix, -1);
        g_free(prefix);
        const UmiBuildDiagnostic *item = &page->items[index];
        const char *severity = item->severity == UMI_BUILD_DIAGNOSTIC_FATAL     ? "fatal"
                               : item->severity == UMI_BUILD_DIAGNOSTIC_ERROR   ? "error"
                               : item->severity == UMI_BUILD_DIAGNOSTIC_WARNING ? "warning"
                                                                                : "note";
        g_string_append_printf(text, "%s:%zu:%zu: %s%s%s\n%s\n\n",
                               item->file[0] != '\0' ? item->file : "(no source file)", item->line,
                               item->column, severity, item->code[0] != '\0' ? " " : "", item->code,
                               item->message);
    }
    if (page->count == 0U)
        g_string_append(text, "No retained diagnostics on this page.");
    char *display = g_utf8_make_valid(text->str, (gssize)text->len);
    g_string_free(text, TRUE);
    char *summary =
        operation == 0U
            ? g_strdup("No running-build diagnostics yet.")
            : g_strdup_printf("%s | %s | records %zu–%zu of %zu | %" G_GUINT64_FORMAT
                              " retention losses | %" G_GUINT64_FORMAT " parser losses%s",
                              umi_build_phase_text(page->progress.phase),
                              page->progress.phase_complete ? "phase ended; read its process result"
                                                            : "running",
                              page->count != 0U ? page->first_index + 1U : page->first_index,
                              page->first_index + page->count, page->retained_count,
                              (guint64)page->retention_dropped,
                              (guint64)page->progress.unrepresented,
                              page->progress.streamed ? "" : " | final preview only");
    /* GTK property notifications can re-enter host code. Retain the panel and
     * fence navigation until the complete copied page is visible. */
    g_object_ref(panel);
    view->changing = 1;
    view->operation = operation;
    view->phase = phase;
    view->first = page->first_index;
    view->requested = page->first_index;
    view->count = page->count;
    view->retained = page->retained_count;
    memcpy(view->record_offsets, offsets, sizeof offsets);
    /* Repeated polling must preserve the user's selection and scroll position
     * when no record on this page changed. */
    if (view->rendered_text == NULL || strcmp(view->rendered_text, display) != 0)
    {
        g_free(view->rendered_text);
        view->rendered_text = g_strdup(display);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view->text)), display, -1);
    }
    gtk_label_set_text(GTK_LABEL(view->summary), summary);
    gtk_widget_set_sensitive(view->previous, view->first != 0U);
    gtk_widget_set_sensitive(view->next, view->count < view->retained - view->first);
    gtk_widget_set_sensitive(view->open_source, view->opener != NULL && view->count != 0U);
    view->changing = 0;
    g_free(display);
    g_free(summary);
    g_object_unref(panel);
    return UMI_STATUS_OK;
}
