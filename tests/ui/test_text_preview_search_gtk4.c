/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_text_preview_search_gtk4.c
 *
 * PURPOSE:
 *   Verify read-only preview search, selection preservation and synchronous observer ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/text_preview.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)
typedef struct SearchObserver {
    GtkWindow *owner; GtkLabel *summary; GtkTextView *view;
    const char *mode; unsigned calls; UmiStatus nested;
} SearchObserver;
static void MarkSet(GtkTextBuffer *buffer, const GtkTextIter *position, GtkTextMark *mark, gpointer data)
{
    (void)position; (void)mark;
    SearchObserver *p = data;
    if (p->calls++ != 0U) return;
    if (!strcmp(p->mode, "select-close")) gtk_window_destroy(p->owner);
    else if (!strcmp(p->mode, "select-reenter"))
        p->nested = UmiGtk4TextPreviewFind(p->owner, p->view, "beta");
    else if (!strcmp(p->mode, "select-publish"))
        p->nested = UmiGtk4TextPreviewPublish(p->owner, p->summary, p->view, "Other", "Replacement");
    else if (!strcmp(p->mode, "select-replace")) {
        GtkTextBuffer *replacement = gtk_text_buffer_new(NULL);
        gtk_text_view_set_buffer(p->view, replacement); g_object_unref(replacement);
    } else if (!strcmp(p->mode, "select-text")) gtk_text_buffer_set_text(buffer, "Observer text", -1);
    else if (!strcmp(p->mode, "select-selection")) {
        GtkTextIter end; gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_place_cursor(buffer, &end);
    }
}
static int Selection(GtkTextBuffer *buffer, int first, int last)
{
    GtkTextIter start, end;
    int selected = gtk_text_buffer_get_selection_bounds(buffer, &start, &end);
    if (first == last) {
        gtk_text_buffer_get_iter_at_mark(buffer, &start, gtk_text_buffer_get_insert(buffer));
        return !selected && gtk_text_iter_get_offset(&start) == first;
    }
    return selected && gtk_text_iter_get_offset(&start) == first && gtk_text_iter_get_offset(&end) == last;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"find", "case", "unicode", "empty", "no-match", "preserve-selection", "invalid", "editable", "foreign", "closed-owner", "select-close", "select-reenter", "select-publish", "select-replace", "select-text", "select-selection", "retained-buffer", "bounds"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    int failed = 0; gulong handler = 0U;
    GtkWindow *owner = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *other = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkLabel *summary = g_object_ref_sink(GTK_LABEL(gtk_label_new("Before")));
    GtkTextView *view = g_object_ref_sink(GTK_TEXT_VIEW(gtk_text_view_new()));
    gtk_text_view_set_editable(view, FALSE);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(summary)); gtk_box_append(GTK_BOX(box), GTK_WIDGET(view));
    gtk_window_set_child(owner, box);
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(view));
    gtk_text_buffer_set_text(buffer, "Alpha beta alpha\ncaf\xc3\xa9", -1);
    gtk_text_buffer_set_modified(buffer, FALSE);
    SearchObserver observer = {owner, summary, view, mode, 0U, UMI_STATUS_OK};
    if (!strcmp(mode, "invalid")) {
        CHECK(UmiGtk4TextPreviewFind(NULL, view, "alpha") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextPreviewFind(owner, view, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextPreviewFind(owner, view, "\xff") == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (!strcmp(mode, "bounds")) {
        char query[1026]; memset(query, 'x', sizeof query); query[1024] = '\0';
        CHECK(UmiGtk4TextPreviewFind(owner, view, query) == UMI_STATUS_NOT_FOUND);
        query[1024] = 'x'; query[1025] = '\0';
        CHECK(UmiGtk4TextPreviewFind(owner, view, query) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (!strcmp(mode, "editable")) gtk_text_view_set_editable(view, TRUE);
    if (!strcmp(mode, "closed-owner")) gtk_window_destroy(owner);
    if (!strcmp(mode, "editable") || !strcmp(mode, "foreign") || !strcmp(mode, "closed-owner")) {
        CHECK(UmiGtk4TextPreviewFind(!strcmp(mode, "foreign") ? other : owner, view, "alpha") == UMI_STATUS_INVALID_STATE);
        goto done;
    }
    if (!strcmp(mode, "empty") || !strcmp(mode, "preserve-selection")) {
        CHECK(UmiGtk4TextPreviewFind(owner, view, "beta") == UMI_STATUS_OK);
        CHECK(Selection(buffer, 6, 10));
    }
    if (!strncmp(mode, "select-", 7U))
        handler = g_signal_connect(buffer, "mark-set", G_CALLBACK(MarkSet), &observer);
    const char *query = !strcmp(mode, "empty") ? "" :
        !strcmp(mode, "unicode") ? "CAF\xc3\x89" :
        !strcmp(mode, "no-match") || !strcmp(mode, "preserve-selection") ? "missing" :
        !strcmp(mode, "case") ? "ALPHA" : "Alpha";
    UmiStatus status = UmiGtk4TextPreviewFind(owner, view, query);
    int cancelled = !strcmp(mode, "select-close") || !strcmp(mode, "select-replace") ||
        !strcmp(mode, "select-text") || !strcmp(mode, "select-selection");
    int missing = !strcmp(mode, "no-match") || !strcmp(mode, "preserve-selection");
    CHECK(status == (cancelled ? UMI_STATUS_CANCELLED : missing ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
    if (handler != 0U) {
        CHECK(observer.calls != 0U); g_signal_handler_disconnect(buffer, handler); handler = 0U;
    }
    if (!strcmp(mode, "select-reenter") || !strcmp(mode, "select-publish"))
        CHECK(observer.nested == UMI_STATUS_BUSY);
    if (!cancelled && !missing) {
        CHECK(Selection(buffer, !strcmp(mode, "unicode") ? 17 : 0,
            !strcmp(mode, "empty") ? 0 : !strcmp(mode, "unicode") ? 21 : 5));
        CHECK(!gtk_text_buffer_get_modified(buffer));
        CHECK(gtk_text_view_get_buffer(view) == buffer);
        CHECK(!strcmp(gtk_label_get_text(summary), "Before"));
    }
    if (!strcmp(mode, "preserve-selection")) CHECK(Selection(buffer, 6, 10));
    if (!strcmp(mode, "retained-buffer")) {
        gtk_window_destroy(owner);
        CHECK(Selection(buffer, 0, 5));
    }
    if (!strcmp(mode, "select-reenter") || !strcmp(mode, "select-publish"))
        CHECK(UmiGtk4TextPreviewFind(owner, view, "beta") == UMI_STATUS_OK);
done:
    if (handler != 0U) g_signal_handler_disconnect(buffer, handler);
    gtk_window_destroy(owner); gtk_window_destroy(other);
    g_object_unref(buffer); g_object_unref(summary); g_object_unref(view);
    g_object_unref(owner); g_object_unref(other);
    return failed;
}
