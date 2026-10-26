/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_forward_practice_native.c
 *
 * PURPOSE:
 *   Exercise local treasury UI admission, stale review and native host retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/forward_practice.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(widget != NULL);
    return widget;
}
static void Entry(GtkWindow *window, const char *name, const char *text) {
    GtkWidget *field = Find(window, name); CHECK(GTK_IS_EDITABLE(field));
    gtk_editable_set_text(GTK_EDITABLE(field), text);
}
static void Click(GtkWindow *window, const char *name) {
    GtkWidget *button = Find(window, name); CHECK(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
}
static void Read(GtkWindow *window, UmiOpenTmsFxForwardBook *book) {
    CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_OK);
}
static void Prepare(GtkWindow *window, guint next) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "treasury.forward.next")), next);
    Click(window, "treasury.forward.prepare");
}
static void Accept(GtkWindow *window) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.forward.ack")), TRUE);
}
typedef struct Probe {
    GtkWindow *window;
    GtkWidget *selected, *apply, *capture;
    const char *mode;
    unsigned calls;
} Probe;
static void PreviewNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec;
    Probe *probe = value;
    const char *text = gtk_label_get_text(GTK_LABEL(object));
    if (strstr(text, " -> ") == NULL || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "close-preview")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->selected), "missing");
}
static void ApplyNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec;
    Probe *probe = value;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "ack-edit")) gtk_editable_set_text(GTK_EDITABLE(probe->selected), "missing");
    else g_signal_emit_by_name(probe->apply, "clicked");
}
static void RowsChanged(GtkTextBuffer *buffer, gpointer value) {
    (void)buffer;
    Probe *probe = value;
    if (probe->calls++ == 0U) g_signal_emit_by_name(probe->capture, "clicked");
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    GtkWindow *window = UmiGtk4ForwardPracticeCreate(parent); CHECK(window != NULL); g_object_ref(window);
    UmiOpenTmsFxForwardBook *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(book != NULL && before != NULL);
    GtkWidget *apply = Find(window, "treasury.forward.apply"); g_object_ref(apply);
    GtkWidget *capture = Find(window, "treasury.forward.capture"); g_object_ref(capture);
    GtkWidget *selected = Find(window, "treasury.forward.selected"); g_object_ref(selected);
    Probe probe = {window, selected, apply, capture, mode, 0U};
    Entry(window, "treasury.forward.id", "practice-1");
    if (!strcmp(mode, "invalid-nan")) Entry(window, "treasury.forward.notional", "nan");
    else if (!strcmp(mode, "invalid-hex")) Entry(window, "treasury.forward.notional", "0x10");
    else if (!strcmp(mode, "invalid-space")) Entry(window, "treasury.forward.notional", " 100");
    else if (!strcmp(mode, "invalid-comma")) Entry(window, "treasury.forward.notional", "1,25");
    else if (!strcmp(mode, "invalid-currency")) Entry(window, "treasury.forward.base", "gbp");
    else if (!strcmp(mode, "invalid-same-currency")) Entry(window, "treasury.forward.quote", "GBP");
    else if (!strcmp(mode, "invalid-notional")) Entry(window, "treasury.forward.notional", "0");
    if (!strcmp(mode, "capture-nested")) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.forward.rows")));
        g_signal_connect(buffer, "changed", G_CALLBACK(RowsChanged), &probe);
    }
    Click(window, "treasury.forward.capture");
    Read(window, book);
    if (!strncmp(mode, "invalid-", 8U) && strcmp(mode, "invalid-api")) {
        CHECK(book->count == 0U && book->revision == 1U);
    } else {
        CHECK(book->count == 1U && book->revision == 2U);
        CHECK(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
        if (!strcmp(mode, "capture") || !strcmp(mode, "zero-market") || !strcmp(mode, "capture-nested")) {
            CHECK(book->trades[0].marketForwardRate == 0.0);
            if (!strcmp(mode, "capture-nested")) CHECK(probe.calls >= 1U);
        } else if (!strcmp(mode, "duplicate")) {
            memcpy(before, book, sizeof *before);
            Click(window, "treasury.forward.capture");
            Read(window, book); CHECK(memcmp(book, before, sizeof *book) == 0);
        } else if (!strcmp(mode, "parent")) {
            gtk_window_destroy(parent);
            memcpy(before, book, sizeof *before);
            g_signal_emit_by_name(capture, "clicked"); g_signal_emit_by_name(apply, "clicked");
            CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(book, before, sizeof *book) == 0);
            CHECK(UmiGtk4ForwardPracticeCreate(parent) == NULL);
        } else if (!strcmp(mode, "invalid-api")) {
            memcpy(before, book, sizeof *before);
            CHECK(UmiGtk4ForwardPracticeRead(NULL, book) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiGtk4ForwardPracticeRead(window, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiGtk4ForwardPracticeRead(parent, book) == UMI_STATUS_INVALID_STATE);
            CHECK(memcmp(book, before, sizeof *book) == 0);
        } else {
            if (!strcmp(mode, "edit-preview") || !strcmp(mode, "close-preview"))
                g_signal_connect(Find(window, "treasury.forward.preview"), "notify::label", G_CALLBACK(PreviewNotify), &probe);
            Prepare(window, !strcmp(mode, "skip") ? 2U : !strcmp(mode, "cancel") ? 3U : 0U);
            if (!strcmp(mode, "close-preview")) {
                CHECK(probe.calls >= 1U);
                CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
                g_signal_emit_by_name(apply, "clicked");
            } else {
                if (!strcmp(mode, "changed-next")) gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "treasury.forward.next")), 3U);
                if (!strcmp(mode, "changed-form")) Entry(window, "treasury.forward.rate", "1.30");
                if (!strcmp(mode, "changed-selected")) Entry(window, "treasury.forward.selected", "missing");
                if (!strcmp(mode, "ack-edit") || !strcmp(mode, "ack-click"))
                    g_signal_connect(apply, "notify::sensitive", G_CALLBACK(ApplyNotify), &probe);
                if (strcmp(mode, "unacknowledged")) Accept(window);
                gboolean refuse = !strcmp(mode, "skip") || !strcmp(mode, "unacknowledged") ||
                    !strcmp(mode, "edit-preview") || !strncmp(mode, "changed-", 8U) || !strcmp(mode, "ack-edit");
                if (!strcmp(mode, "ack-click")) {
                    Read(window, book); CHECK(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_DRAFT);
                    CHECK(probe.calls >= 1U);
                }
                CHECK(gtk_widget_get_sensitive(apply) == !refuse);
                g_signal_emit_by_name(apply, "clicked");
                Read(window, book);
                CHECK(book->trades[0].state == (refuse ? UMI_OPEN_TMS_FX_FORWARD_DRAFT :
                    !strcmp(mode, "cancel") ? UMI_OPEN_TMS_FX_FORWARD_CANCELLED : UMI_OPEN_TMS_FX_FORWARD_CONFIRMED));
                memcpy(before, book, sizeof *before);
                g_signal_emit_by_name(apply, "clicked"); Read(window, book);
                CHECK(memcmp(book, before, sizeof *book) == 0);
                if (!strcmp(mode, "lifecycle")) {
                    Prepare(window, 1U); Accept(window); Click(window, "treasury.forward.apply");
                    Prepare(window, 2U); Accept(window); Click(window, "treasury.forward.apply");
                    Read(window, book);
                    CHECK(book->trades[0].state == UMI_OPEN_TMS_FX_FORWARD_SETTLED && book->trades[0].version == 4U);
                    Prepare(window, 3U); Accept(window);
                    CHECK(!gtk_widget_get_sensitive(apply));
                }
            }
        }
    }
    gtk_window_destroy(window); gtk_window_destroy(parent);
    g_object_unref(selected); g_object_unref(capture); g_object_unref(apply);
    g_object_unref(window); g_object_unref(parent);
    free(before); free(book);
    return 0;
}
