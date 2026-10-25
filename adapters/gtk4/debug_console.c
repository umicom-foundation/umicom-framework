/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_console.c
 *
 * PURPOSE:
 *   Keep native debugger output pages bound to one copied query and retained GTK owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/debug_console.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>
typedef struct ConsolePanel {
    GtkWidget *root, *session, *category, *text, *severity, *sort, *status;
    GtkTextBuffer *buffer;
    UmiGtk4DebugConsoleQuery query;
    void *context;
    GDestroyNotify destroy;
    UmiDebugConsoleReport *report;
    size_t offset;
    char *rendered;
    gboolean pending;
} ConsolePanel;
static void ConsoleFree(gpointer data)
{
    ConsolePanel *s = data;
    if (s->destroy != NULL) s->destroy(s->context);
    UmiDebugConsoleReportDestroy(s->report);
    g_object_unref(s->session); g_object_unref(s->category); g_object_unref(s->text);
    g_object_unref(s->severity); g_object_unref(s->sort); g_object_unref(s->status);
    g_object_unref(s->buffer); g_free(s->rendered); g_free(s);
}
static gboolean Active(ConsolePanel *s, GtkWidget *button)
{
    return !s->pending && gtk_widget_get_mapped(s->root) &&
        gtk_widget_get_mapped(button) && gtk_widget_is_ancestor(button, s->root);
}
static void ConsoleClear(ConsolePanel *s)
{
    UmiDebugConsoleReportDestroy(s->report); s->report = NULL;
    g_clear_pointer(&s->rendered, g_free); s->offset = 0U;
    gtk_text_buffer_set_text(s->buffer, "", -1);
}
static void ConsoleRender(ConsolePanel *s)
{
    size_t bytes = 0U;
    UmiStatus status = UmiDebugConsoleReportFormat(s->report, s->offset, 64U, NULL, 0U, &bytes);
    char *text = NULL;
    if (status == UMI_STATUS_OK) {
        text = g_malloc(bytes + 1U);
        status = UmiDebugConsoleReportFormat(s->report, s->offset, 64U, text, bytes + 1U, &bytes);
    }
    g_free(s->rendered);
    s->rendered = status == UMI_STATUS_OK ? text : NULL;
    if (status != UMI_STATUS_OK) g_free(text);
    gtk_text_buffer_set_text(s->buffer, s->rendered ? s->rendered : "", -1);
    if (gtk_widget_get_mapped(s->root))
        gtk_label_set_text(GTK_LABEL(s->status), status == UMI_STATUS_OK ?
            "Results from the last Refresh. Draft filters do not change these pages. Copy shares only this page." :
            "This page could not be formatted. Refresh the output capture.");
}
static gboolean FilterText(GtkWidget *entry, char *out, size_t capacity)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    size_t bytes = strlen(text);
    if (bytes >= capacity) return FALSE;
    memcpy(out, text, bytes + 1U);
    return TRUE;
}
static void ConsoleRefresh(GtkButton *button, gpointer data)
{
    GtkWidget *root = data;
    ConsolePanel *s = g_object_get_data(G_OBJECT(root), "umicom-debug-console");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(root); s->pending = TRUE;
    UmiDebugConsoleFilter filter; umi_debug_console_filter_init(&filter);
    guint severity = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->severity));
    guint order = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->sort));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    /* A rejected draft must not redisplay an earlier successful query. */
    UmiDebugConsoleReportDestroy(s->report); s->report = NULL;
    if (severity <= 4U && order <= 2U &&
        FilterText(s->session, filter.session_id, sizeof filter.session_id) &&
        FilterText(s->category, filter.category, sizeof filter.category) &&
        FilterText(s->text, filter.text, sizeof filter.text)) {
        filter.minimum_severity = (int)severity;
        filter.sort = (UmiDebugConsoleSort)(order + 1U);
        filter.limit = 0U; /* Pagination operates on the complete captured query. */
        UmiDebugConsoleReport *report = NULL;
        status = s->query(s->context, &filter, &report);
        if (status == UMI_STATUS_OK && report == NULL) status = UMI_STATUS_INVALID_STATE;
        UmiDebugConsoleReportDestroy(s->report); s->report = NULL;
        if (status == UMI_STATUS_OK && gtk_widget_get_mapped(root)) s->report = report;
        else UmiDebugConsoleReportDestroy(report);
    }
    s->offset = 0U;
    if (s->report != NULL && gtk_widget_get_mapped(root)) ConsoleRender(s);
    else {
        ConsoleClear(s);
        if (gtk_widget_get_mapped(root)) gtk_label_set_text(GTK_LABEL(s->status),
            status == UMI_STATUS_INVALID_ARGUMENT ? "Check filter lengths and choices, then Refresh again." :
            "Output refresh failed or its owner closed. Previous displayed results were cleared.");
    }
    s->pending = FALSE; g_object_unref(root);
}
static void ConsolePage(GtkButton *button, gpointer data)
{
    GtkWidget *root = data; ConsolePanel *s = g_object_get_data(G_OBJECT(root), "umicom-debug-console");
    if (!Active(s, GTK_WIDGET(button)) || s->report == NULL) return;
    g_object_ref(root); s->pending = TRUE;
    UmiDebugConsoleQuerySnapshot summary;
    UmiStatus status = UmiDebugConsoleReportRead(s->report, &summary);
    int direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-direction"));
    if (status == UMI_STATUS_OK) {
        if (direction > 0 && s->offset < summary.result_count && summary.result_count - s->offset > 64U) s->offset += 64U;
        else if (direction < 0) s->offset = s->offset >= 64U ? s->offset - 64U : 0U;
        ConsoleRender(s);
    }
    s->pending = FALSE; g_object_unref(root);
}
static void ConsoleForget(GtkButton *button, gpointer data)
{
    GtkWidget *root = data; ConsolePanel *s = g_object_get_data(G_OBJECT(root), "umicom-debug-console");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(root); s->pending = TRUE; ConsoleClear(s);
    if (gtk_widget_get_mapped(root)) gtk_label_set_text(GTK_LABEL(s->status), "View cleared. Refresh can capture retained debugger output again.");
    s->pending = FALSE; g_object_unref(root);
}
static void ConsoleCopy(GtkButton *button, gpointer data)
{
    GtkWidget *root = data; ConsolePanel *s = g_object_get_data(G_OBJECT(root), "umicom-debug-console");
    if (!Active(s, GTK_WIDGET(button)) || s->rendered == NULL) return;
    g_object_ref(root); s->pending = TRUE;
    GdkClipboard *clipboard = gtk_widget_get_clipboard(GTK_WIDGET(button));
    if (clipboard != NULL) gdk_clipboard_set_text(clipboard, s->rendered);
    if (gtk_widget_get_mapped(root)) gtk_label_set_text(GTK_LABEL(s->status), clipboard ?
        "Displayed output page copied to clipboard." : "Clipboard unavailable.");
    s->pending = FALSE; g_object_unref(root);
}
static GtkWidget *Field(ConsolePanel *s, const char *label, const char *tag, int capacity)
{
    GtkWidget *entry = gtk_entry_new(); g_object_ref_sink(entry);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), label); gtk_entry_set_max_length(GTK_ENTRY(entry), capacity);
    (void)umi_gtk4_automation_tag_widget(entry, tag); gtk_box_append(GTK_BOX(s->root), entry);
    return entry;
}
static void Button(ConsolePanel *s, GtkWidget *row, const char *label, const char *tag,
    GCallback callback, int direction)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, tag);
    g_object_set_data(G_OBJECT(button), "page-direction", GINT_TO_POINTER(direction));
    g_signal_connect_object(button, "clicked", callback, s->root, 0);
    gtk_box_append(GTK_BOX(row), button);
}
GtkWidget *UmiGtk4DebugConsoleCreate(UmiGtk4DebugConsoleQuery query, void *context, GDestroyNotify destroy)
{
    if (query == NULL) return NULL;
    ConsolePanel *s = g_new0(ConsolePanel, 1); s->query = query; s->context = context; s->destroy = destroy;
    s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    s->session = Field(s, "Exact session ID (empty: all retained sessions)", "debug.console.session", 127);
    s->category = Field(s, "Exact category (empty: all)", "debug.console.category", 63);
    s->text = Field(s, "Text contains (bytewise case-insensitive)", "debug.console.filter", 511);
    const char *severities[] = {"All severities", "At least 1", "At least 2 (warnings)", "At least 3 (errors)", "At least 4", NULL};
    const char *orders[] = {"Oldest first", "Newest first", "Severity first", NULL};
    s->severity = gtk_drop_down_new_from_strings(severities); g_object_ref_sink(s->severity);
    s->sort = gtk_drop_down_new_from_strings(orders); g_object_ref_sink(s->sort);
    (void)umi_gtk4_automation_tag_widget(s->severity, "debug.console.severity");
    (void)umi_gtk4_automation_tag_widget(s->sort, "debug.console.sort");
    gtk_box_append(GTK_BOX(s->root), s->severity); gtk_box_append(GTK_BOX(s->root), s->sort);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    Button(s, row, "Refresh output", "debug.console.refresh", G_CALLBACK(ConsoleRefresh), 0);
    Button(s, row, "Previous", "debug.console.previous", G_CALLBACK(ConsolePage), -1);
    Button(s, row, "Next", "debug.console.next", G_CALLBACK(ConsolePage), 1);
    Button(s, row, "Copy page", "debug.console.copy", G_CALLBACK(ConsoleCopy), 0);
    Button(s, row, "Clear view", "debug.console.clear", G_CALLBACK(ConsoleForget), 0);
    gtk_box_append(GTK_BOX(s->root), row);
    s->buffer = gtk_text_buffer_new(NULL);
    GtkWidget *view = gtk_text_view_new_with_buffer(s->buffer), *scroll = gtk_scrolled_window_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE); gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    (void)umi_gtk4_automation_tag_widget(view, "debug.console.output");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 180);
    gtk_box_append(GTK_BOX(s->root), scroll);
    s->status = gtk_label_new("Refresh captures retained debugger output. Filters and pages never evaluate expressions.");
    g_object_ref_sink(s->status); gtk_label_set_wrap(GTK_LABEL(s->status), TRUE);
    (void)umi_gtk4_automation_tag_widget(s->status, "debug.console.status");
    gtk_box_append(GTK_BOX(s->root), s->status);
    (void)umi_gtk4_automation_tag_widget(s->root, "debug.console.panel");
    g_object_set_data_full(G_OBJECT(s->root), "umicom-debug-console", s, ConsoleFree);
    return s->root;
}
