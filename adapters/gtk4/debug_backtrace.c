/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_backtrace.c
 *
 * PURPOSE:
 *   Own native backtrace review state across notification, clipboard and window-close boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/debug_backtrace.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include "umicom/ui/gtk4/automation.h"
#include "window_removal_private.h"
typedef struct BacktraceWindow {
    GtkWindow *window;
    UmiDebugBacktrace *trace;
    GtkWidget *paths, *status;
    GtkTextBuffer *buffer;
    char *rendered;
    gboolean pending;
} BacktraceWindow;
static void BacktraceFree(gpointer data)
{
    BacktraceWindow *state = data;
    UmiDebugBacktraceDestroy(state->trace);
    g_clear_object(&state->paths);
    g_clear_object(&state->status);
    g_clear_object(&state->buffer);
    g_free(state->rendered);
    g_free(state);
}
static void BacktraceRefresh(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    GtkWindow *window = data;
    BacktraceWindow *state = g_object_get_data(G_OBJECT(window), "umicom-debug-backtrace");
    if (state->pending || !UmiGtk4WindowIsOpen(window)) return;
    g_object_ref(window);
    state->pending = TRUE;
    guint mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->paths));
    size_t required = 0U;
    UmiStatus status = UmiDebugBacktraceFormat(state->trace,
        (UmiDebugBacktracePaths)mode, NULL, 0U, &required);
    char *text = NULL;
    if (status == UMI_STATUS_OK) {
        text = g_malloc(required + 1U);
        status = UmiDebugBacktraceFormat(state->trace, (UmiDebugBacktracePaths)mode,
            text, required + 1U, &required);
    }
    g_free(state->rendered);
    state->rendered = status == UMI_STATUS_OK ? text : NULL;
    if (status != UMI_STATUS_OK) g_free(text);
    /* Publish the owned copy before notifying buffer observers. A close callback
     * may remove the widgets, but this call still holds their state and buffer. */
    gtk_text_buffer_set_text(state->buffer, state->rendered ? state->rendered : "", -1);
    if (UmiGtk4WindowIsOpen(window))
        gtk_label_set_text(GTK_LABEL(state->status), status == UMI_STATUS_OK ?
            "Captured frames only. Review the text, then copy if needed. Names may contain private information." :
            "Cannot format this capture. No text is available to copy.");
    /* Native notifications may replace the selected path policy. Do not leave
     * full-path text presented beneath a newer omit-paths choice. Clear it and
     * require an explicit selection change instead of recursively refreshing. */
    if (UmiGtk4WindowIsOpen(window) &&
        gtk_drop_down_get_selected(GTK_DROP_DOWN(state->paths)) != mode) {
        g_clear_pointer(&state->rendered, g_free);
        gtk_text_buffer_set_text(state->buffer, "", -1);
        if (UmiGtk4WindowIsOpen(window))
            gtk_label_set_text(GTK_LABEL(state->status), "Path display changed during refresh. Select the desired mode again.");
    }
    state->pending = FALSE;
    g_object_unref(window);
}
static void BacktraceCopy(GtkButton *button, gpointer data)
{
    GtkWindow *window = data;
    BacktraceWindow *state = g_object_get_data(G_OBJECT(window), "umicom-debug-backtrace");
    if (state->pending || state->rendered == NULL || !UmiGtk4WindowIsOpen(window) ||
        gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window) ||
        !gtk_widget_get_mapped(GTK_WIDGET(button))) return;
    g_object_ref(window);
    state->pending = TRUE;
    GdkClipboard *clipboard = gtk_widget_get_clipboard(GTK_WIDGET(button));
    if (clipboard != NULL) {
        gdk_clipboard_set_text(clipboard, state->rendered);
        if (UmiGtk4WindowIsOpen(window))
            gtk_label_set_text(GTK_LABEL(state->status), "Reviewed capture copied to the clipboard.");
    }
    state->pending = FALSE;
    g_object_unref(window);
}
UmiStatus UmiGtk4DebugBacktraceCreateWindow(GtkWindow *owner,
    const UmiDebugBacktrace *trace, GtkWindow **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (!GTK_IS_WINDOW(owner) || trace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiGtk4WindowIsOpen(owner)) return UMI_STATUS_CANCELLED;
    GtkApplication *app = gtk_window_get_application(owner);
    if (app == NULL) return UMI_STATUS_INVALID_STATE;
    BacktraceWindow *state = g_new0(BacktraceWindow, 1);
    UmiStatus status = UmiDebugBacktraceClone(trace, &state->trace);
    if (status != UMI_STATUS_OK) { g_free(state); return status; }
    g_object_ref(owner); g_object_ref(app);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    state->window = window;
    state->pending = TRUE;
    GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    const char *modes[] = {"Omit source paths", "Source filenames", "Full source paths", NULL};
    state->paths = gtk_drop_down_new_from_strings(modes);
    g_object_ref_sink(state->paths);
    state->status = gtk_label_new("Review captured call stack");
    g_object_ref_sink(state->status);
    state->buffer = gtk_text_buffer_new(NULL);
    GtkWidget *view = gtk_text_view_new_with_buffer(state->buffer);
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *copy = gtk_button_new_with_label("Copy reviewed text");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
    gtk_box_append(GTK_BOX(body), state->paths);
    gtk_box_append(GTK_BOX(body), scroll);
    gtk_box_append(GTK_BOX(body), copy);
    gtk_box_append(GTK_BOX(body), state->status);
    gtk_widget_set_name(GTK_WIDGET(window), "debug.backtrace.review");
    (void)umi_gtk4_automation_tag_widget(state->paths, "debug.backtrace.paths");
    (void)umi_gtk4_automation_tag_widget(view, "debug.backtrace.text");
    (void)umi_gtk4_automation_tag_widget(copy, "debug.backtrace.copy");
    (void)umi_gtk4_automation_tag_widget(state->status, "debug.backtrace.status");
    g_object_set_data_full(G_OBJECT(window), "umicom-debug-backtrace", state, BacktraceFree);
    gtk_window_set_child(window, body);
    gtk_window_set_title(window, "Captured call stack");
    gtk_window_set_default_size(window, 780, 520);
    gtk_window_set_transient_for(window, owner);
    gtk_window_set_destroy_with_parent(window, TRUE);
    UmiGtk4ObserveWindowRemoval(owner, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
    /* Set application only after state ownership is established: window-added
     * observers can synchronously close either owner or this review. */
    if (UmiGtk4WindowIsOpen(window)) gtk_window_set_application(window, app);
    if (!UmiGtk4WindowIsOpen(owner) || !UmiGtk4WindowIsOpen(window)) status = UMI_STATUS_CANCELLED;
    else {
        g_signal_connect_object(state->paths, "notify::selected", G_CALLBACK(BacktraceRefresh), window, 0);
        g_signal_connect_object(copy, "clicked", G_CALLBACK(BacktraceCopy), window, 0);
        state->pending = FALSE;
        BacktraceRefresh(NULL, NULL, window);
        if (!UmiGtk4WindowIsOpen(window)) status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK) *out = window;
    else gtk_window_destroy(window);
    g_object_unref(window); g_object_unref(app); g_object_unref(owner);
    return status;
}
