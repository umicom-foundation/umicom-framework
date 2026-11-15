/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui/test_text_preview_gtk4.c
 *
 * PURPOSE:
 *   Verify owned read-only publication against retained buffers and synchronous native observers.
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
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto done; } } while (0)
typedef struct PreviewObserver {
    GtkWindow *owner; GtkLabel *summary; GtkTextView *view; GtkWidget *box;
    const char *mode; unsigned calls; UmiStatus nested;
} PreviewObserver;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    PreviewObserver *p = data;
    if (p->calls++ != 0U) return;
    if (strstr(p->mode, "close")) gtk_window_destroy(p->owner);
    else if (strstr(p->mode, "reenter")) p->nested = UmiGtk4TextPreviewPublish(p->owner, p->summary, p->view, "Nested", "Nested");
    else if (!strcmp(p->mode, "buffer-replace")) {
        GtkTextBuffer *other = gtk_text_buffer_new(NULL); gtk_text_view_set_buffer(p->view, other); g_object_unref(other);
    } else if (!strcmp(p->mode, "buffer-editable")) gtk_text_view_set_editable(p->view, TRUE);
    else if (!strcmp(p->mode, "buffer-content")) gtk_text_buffer_set_text(gtk_text_view_get_buffer(p->view), "Observer", -1);
    else if (!strcmp(p->mode, "label-content") || !strcmp(p->mode, "alias")) gtk_label_set_text(p->summary, "Observer");
    else if (!strcmp(p->mode, "label-detach")) gtk_box_remove(GTK_BOX(p->box), GTK_WIDGET(p->summary));
}
static char *Text(GtkTextBuffer *buffer)
{
    GtkTextIter start, end; gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
}
int main(int argc, char **argv)
{
    const char *cases[] = {"publish", "repeat", "empty", "invalid", "editable", "foreign", "closed-owner",
        "unicode", "retained-buffer", "alias", "buffer-close", "buffer-reenter", "buffer-replace",
        "buffer-editable", "buffer-content", "label-close", "label-reenter", "label-content", "label-detach"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0; const char *mode = argv[1]; gulong handler = 0U;
    GtkWindow *owner = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWindow *other = g_object_ref(GTK_WINDOW(gtk_window_new()));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkLabel *summary = g_object_ref_sink(GTK_LABEL(gtk_label_new("Before")));
    GtkTextView *view = g_object_ref_sink(GTK_TEXT_VIEW(gtk_text_view_new()));
    gtk_text_view_set_editable(view, FALSE);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(summary)); gtk_box_append(GTK_BOX(box), GTK_WIDGET(view));
    gtk_window_set_child(owner, box);
    GtkTextBuffer *old = g_object_ref(gtk_text_view_get_buffer(view));
    gtk_text_buffer_set_text(old, "Original", -1);
    char *value = NULL;
    PreviewObserver observer = {owner, summary, view, box, mode, 0U, UMI_STATUS_OK};
    GObject *observed = NULL;
    if (!strcmp(mode, "invalid")) {
        CHECK(UmiGtk4TextPreviewPublish(NULL, summary, view, "Caption", "Text") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextPreviewPublish(owner, summary, view, NULL, "Text") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4TextPreviewPublish(owner, summary, view, "Caption", "\xff") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(gtk_text_view_get_buffer(view) == old); goto done;
    }
    if (!strcmp(mode, "editable")) gtk_text_view_set_editable(view, TRUE);
    if (!strcmp(mode, "closed-owner")) gtk_window_destroy(owner);
    if (!strcmp(mode, "editable") || !strcmp(mode, "closed-owner") || !strcmp(mode, "foreign")) {
        CHECK(UmiGtk4TextPreviewPublish(!strcmp(mode, "foreign") ? other : owner, summary, view,
            "Caption", "Text") == UMI_STATUS_INVALID_STATE);
        CHECK(gtk_text_view_get_buffer(view) == old); goto done;
    }
    if (!strncmp(mode, "buffer-", 7U) || !strcmp(mode, "alias")) {
        observed = G_OBJECT(view); handler = g_signal_connect(view, "notify::buffer", G_CALLBACK(Observe), &observer);
    } else if (!strncmp(mode, "label-", 6U)) {
        observed = G_OBJECT(summary); handler = g_signal_connect(summary, "notify::label", G_CALLBACK(Observe), &observer);
    }
    const char *caption = !strcmp(mode, "alias") ? gtk_label_get_text(summary) : "Caption";
    const char *body = !strcmp(mode, "empty") ? "" : !strcmp(mode, "unicode") ? "\xc3\xa9\nsource" : "Complete text\nsecond line";
    UmiStatus status = UmiGtk4TextPreviewPublish(owner, summary, view, caption, body);
    int cancelled = strstr(mode, "close") != NULL || strstr(mode, "replace") != NULL ||
        strstr(mode, "editable") != NULL || strstr(mode, "content") != NULL || strstr(mode, "detach") != NULL;
    CHECK(status == (cancelled ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
    if (handler != 0U) { CHECK(observer.calls != 0U); g_signal_handler_disconnect(observed, handler); handler = 0U; }
    if (strstr(mode, "reenter")) CHECK(observer.nested == UMI_STATUS_BUSY);
    if (!strcmp(mode, "retained-buffer")) gtk_window_destroy(owner);
    value = Text(old); CHECK(!strcmp(value, "Original")); g_clear_pointer(&value, g_free);
    if (!cancelled) {
        CHECK(gtk_text_view_get_buffer(view) != old);
        value = Text(gtk_text_view_get_buffer(view)); CHECK(!strcmp(value, body)); g_clear_pointer(&value, g_free);
        CHECK(!strcmp(gtk_label_get_text(summary), !strcmp(mode, "alias") ? "Before" : "Caption"));
        CHECK(!gtk_text_buffer_get_modified(gtk_text_view_get_buffer(view)));
    }
    if (!strcmp(mode, "repeat") || strstr(mode, "reenter"))
        CHECK(UmiGtk4TextPreviewPublish(owner, summary, view, "Again", "Again") == UMI_STATUS_OK);
done:
    if (handler != 0U) g_signal_handler_disconnect(observed, handler);
    g_free(value); g_object_unref(old); gtk_window_destroy(owner); gtk_window_destroy(other);
    g_object_unref(summary); g_object_unref(view); g_object_unref(owner); g_object_unref(other);
    return failed;
}
