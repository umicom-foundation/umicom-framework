/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/text_preview.c
 *
 * PURPOSE:
 *   Retain native preview owners and recheck observable buffer/summary publication before reporting success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_preview.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
#define PREVIEW_GUARD "umicom-text-preview-publish-active"
static int PreviewOwned(GtkWindow *owner, GtkLabel *summary, GtkTextView *view)
{
    return UmiGtk4WindowIsOpen(owner) &&
        gtk_widget_get_root(GTK_WIDGET(summary)) == GTK_ROOT(owner) &&
        gtk_widget_get_root(GTK_WIDGET(view)) == GTK_ROOT(owner) &&
        !gtk_text_view_get_editable(view);
}
static int PreviewLength(const char *text, size_t limit, size_t *length)
{
    if (text == NULL) return 0;
    size_t n = 0U;
    while (n <= limit && text[n] != '\0') ++n;
    if (n > limit || !g_utf8_validate(text, (gssize)n, NULL)) return 0;
    *length = n;
    return 1;
}
static int PreviewUnchanged(GtkTextBuffer *buffer, const char *expected)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *current = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    int equal = strcmp(current, expected) == 0;
    g_free(current);
    return equal;
}
UmiStatus UmiGtk4TextPreviewPublish(GtkWindow *owner, GtkLabel *summary,
    GtkTextView *view, const char *summary_text, const char *text)
{
    if (!GTK_IS_WINDOW(owner) || !GTK_IS_LABEL(summary) || !GTK_IS_TEXT_VIEW(view))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(view), PREVIEW_GUARD) != NULL) return UMI_STATUS_BUSY;
    if (!PreviewOwned(owner, summary, view)) return UMI_STATUS_INVALID_STATE;
    size_t summary_bytes = 0U, text_bytes = 0U;
    if (!PreviewLength(summary_text, 4096U, &summary_bytes) ||
        !PreviewLength(text, UMI_GTK4_TEXT_PREVIEW_MAX_BYTES, &text_bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    char *caption = g_try_malloc(summary_bytes + 1U);
    char *body = g_try_malloc(text_bytes + 1U);
    if (caption == NULL || body == NULL) { g_free(caption); g_free(body); return UMI_STATUS_OUT_OF_MEMORY; }
    memcpy(caption, summary_text, summary_bytes + 1U);
    memcpy(body, text, text_bytes + 1U);
    g_object_ref(owner); g_object_ref(summary); g_object_ref(view);
    g_object_set_data(G_OBJECT(view), PREVIEW_GUARD, GINT_TO_POINTER(1));
    /* Prepare off-screen. Buffer replacement publishes complete text at once;
     * readers holding the old buffer continue to own its original contents. */
    GtkTextBuffer *candidate = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(candidate, body, (gint)text_bytes);
    gtk_text_buffer_set_modified(candidate, FALSE);
    gtk_text_view_set_buffer(view, candidate);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!PreviewOwned(owner, summary, view) || gtk_text_view_get_buffer(view) != candidate ||
        !PreviewUnchanged(candidate, body)) goto done;
    gtk_label_set_text(summary, caption);
    if (PreviewOwned(owner, summary, view) && gtk_text_view_get_buffer(view) == candidate &&
        strcmp(gtk_label_get_text(summary), caption) == 0 && PreviewUnchanged(candidate, body))
        status = UMI_STATUS_OK;
done:
    g_object_set_data(G_OBJECT(view), PREVIEW_GUARD, NULL);
    g_object_unref(candidate);
    g_object_unref(view); g_object_unref(summary); g_object_unref(owner);
    g_free(body); g_free(caption);
    return status;
}

static void PreviewBufferChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    *(int *)data = 1;
}
static int PreviewViewOwned(GtkWindow *owner, GtkTextView *view)
{
    return UmiGtk4WindowIsOpen(owner) &&
        gtk_widget_get_root(GTK_WIDGET(view)) == GTK_ROOT(owner) &&
        !gtk_text_view_get_editable(view);
}
static int PreviewSelectionMatches(GtkTextBuffer *buffer, int first, int last)
{
    GtkTextIter start, end;
    int selected = gtk_text_buffer_get_selection_bounds(buffer, &start, &end);
    if (first == last) {
        gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
        return !selected && gtk_text_iter_get_offset(&start) == first;
    }
    return selected && gtk_text_iter_get_offset(&start) == first &&
        gtk_text_iter_get_offset(&end) == last;
}
UmiStatus UmiGtk4TextPreviewFind(GtkWindow *owner, GtkTextView *view, const char *query)
{
    if (!GTK_IS_WINDOW(owner) || !GTK_IS_TEXT_VIEW(view)) return UMI_STATUS_INVALID_ARGUMENT;
    if (g_object_get_data(G_OBJECT(view), PREVIEW_GUARD) != NULL) return UMI_STATUS_BUSY;
    if (!PreviewViewOwned(owner, view)) return UMI_STATUS_INVALID_STATE;
    size_t bytes = 0U;
    if (!PreviewLength(query, 1024U, &bytes)) return UMI_STATUS_INVALID_ARGUMENT;
    char *copied = g_try_malloc(bytes + 1U);
    if (copied == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copied, query, bytes + 1U);
    g_object_ref(owner); g_object_ref(view);
    g_object_set_data(G_OBJECT(view), PREVIEW_GUARD, GINT_TO_POINTER(1));
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(view));
    UmiStatus status = UMI_STATUS_CANCELLED;
    int changed = 0;
    gulong handler = g_signal_connect(buffer, "changed", G_CALLBACK(PreviewBufferChanged), &changed);
    if (!PreviewViewOwned(owner, view) || gtk_text_view_get_buffer(view) != buffer) goto done;
    GtkTextIter start, match_start, match_end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    int first = 0, last = 0;
    if (bytes != 0U) {
        if (!gtk_text_iter_forward_search(&start, copied, GTK_TEXT_SEARCH_CASE_INSENSITIVE,
                &match_start, &match_end, NULL)) { status = UMI_STATUS_NOT_FOUND; goto done; }
        first = gtk_text_iter_get_offset(&match_start);
        last = gtk_text_iter_get_offset(&match_end);
        gtk_text_buffer_select_range(buffer, &match_start, &match_end);
    } else gtk_text_buffer_place_cursor(buffer, &start);
    /* Selection observers can invalidate every iterator. Only integer offsets
     * cross that boundary; reconstruct an iterator after identity/content checks. */
    if (!PreviewViewOwned(owner, view) || gtk_text_view_get_buffer(view) != buffer ||
        changed || !PreviewSelectionMatches(buffer, first, last)) goto done;
    gtk_text_buffer_get_iter_at_offset(buffer, &start, first);
    (void)gtk_text_view_scroll_to_iter(view, &start, 0.15, FALSE, 0.0, 0.0);
    if (PreviewViewOwned(owner, view) && gtk_text_view_get_buffer(view) == buffer &&
        !changed && PreviewSelectionMatches(buffer, first, last)) status = UMI_STATUS_OK;
done:
    g_signal_handler_disconnect(buffer, handler);
    g_object_set_data(G_OBJECT(view), PREVIEW_GUARD, NULL);
    g_object_unref(buffer); g_object_unref(view); g_object_unref(owner); g_free(copied);
    return status;
}

#undef PREVIEW_GUARD
