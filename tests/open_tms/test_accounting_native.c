/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_native.c
 *
 * PURPOSE:
 *   Check local accounting UI admission, full-batch review and retained-host refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/accounting_practice.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(widget != NULL); return widget;
}
static void Entry(GtkWindow *window, const char *name, const char *value) {
    gtk_editable_set_text(GTK_EDITABLE(Find(window, name)), value);
}
static void Click(GtkWindow *window, const char *name) {
    g_signal_emit_by_name(Find(window, name), "clicked");
}
static void Read(GtkWindow *window, UmiOpenTmsAccountingBook *book) {
    CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_OK);
}
static void Accept(GtkWindow *window) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.accounting.ack")), TRUE);
}
static void Credit(GtkWindow *window) {
    Entry(window, "treasury.accounting.account", "practice-receivable");
    Entry(window, "treasury.accounting.debit", "0");
    Entry(window, "treasury.accounting.credit", "100");
    Click(window, "treasury.accounting.capture");
}
typedef struct Probe {
    GtkWindow *window;
    GtkWidget *field, *apply, *capture;
    const char *mode;
    unsigned actions;
} Probe;
static void Preview(GtkTextBuffer *buffer, gpointer value) {
    Probe *probe = value;
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    gboolean ready = g_str_has_prefix(text, "Review complete local batch");
    g_free(text);
    if (!ready || probe->actions != 0U) return;
    ++probe->actions;
    if (!strcmp(probe->mode, "close-preview")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed-after-preview");
}
static void Sensitive(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec;
    Probe *probe = value;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->actions != 0U) return;
    ++probe->actions;
    if (!strcmp(probe->mode, "ack-edit")) gtk_editable_set_text(GTK_EDITABLE(probe->field), "later");
    else g_signal_emit_by_name(probe->apply, "clicked");
}
static void Rows(GtkTextBuffer *buffer, gpointer value) {
    (void)buffer;
    Probe *probe = value;
    if (probe->actions != 0U) return;
    ++probe->actions; g_signal_emit_by_name(probe->capture, "clicked");
}
int main(int argc, char **argv) {
    const char *cases[] = {"capture-pair", "approve", "post", "unbalanced", "cross-currency",
        "unacknowledged", "changed-form", "changed-action", "edit-preview", "close-preview",
        "parent", "ack-edit", "ack-click", "capture-nested", "posted-capture", "edit-approved",
        "repeat", "empty", "invalid-api", "invalid-action", "nan", "infinity", "hex", "comma",
        "whitespace", "currency", "empty-reference", "both-sides", "zero-sides"};
    CHECK(argc == 2); int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = 1;
    CHECK(known);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    GtkWindow *window = UmiGtk4AccountingPracticeCreate(parent); CHECK(window); g_object_ref(window);
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book), *before = malloc(sizeof *before); CHECK(book && before);
    GtkWidget *apply = g_object_ref(Find(window, "treasury.accounting.apply"));
    GtkWidget *capture = g_object_ref(Find(window, "treasury.accounting.capture"));
    GtkWidget *field = g_object_ref(Find(window, "treasury.accounting.reference"));
    Probe probe = {window, field, apply, capture, mode, 0U};
    Read(window, book); CHECK(book->count == 0U && book->revision == 1U);
    if (!strcmp(mode, "nan")) Entry(window, "treasury.accounting.debit", "nan");
    if (!strcmp(mode, "infinity")) Entry(window, "treasury.accounting.debit", "inf");
    if (!strcmp(mode, "hex")) Entry(window, "treasury.accounting.debit", "0x10");
    if (!strcmp(mode, "comma")) Entry(window, "treasury.accounting.debit", "1,25");
    if (!strcmp(mode, "whitespace")) Entry(window, "treasury.accounting.debit", " 100");
    if (!strcmp(mode, "currency")) Entry(window, "treasury.accounting.currency", "gbp");
    if (!strcmp(mode, "empty-reference")) Entry(window, "treasury.accounting.reference", "");
    if (!strcmp(mode, "both-sides")) Entry(window, "treasury.accounting.credit", "100");
    if (!strcmp(mode, "zero-sides")) Entry(window, "treasury.accounting.debit", "0");
    gboolean invalid = !strcmp(mode, "nan") || !strcmp(mode, "infinity") || !strcmp(mode, "hex") ||
        !strcmp(mode, "comma") || !strcmp(mode, "whitespace") || !strcmp(mode, "currency") ||
        !strcmp(mode, "empty-reference") || !strcmp(mode, "both-sides") || !strcmp(mode, "zero-sides");
    if (!strcmp(mode, "capture-nested"))
        g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.accounting.rows"))),
            "changed", G_CALLBACK(Rows), &probe);
    if (strcmp(mode, "empty")) Click(window, "treasury.accounting.capture");
    Read(window, book);
    CHECK(book->count == (invalid || !strcmp(mode, "empty") ? 0U : 1U));
    if (invalid || !strcmp(mode, "empty")) {
        Click(window, "treasury.accounting.prepare"); Accept(window); g_signal_emit_by_name(apply, "clicked");
        Read(window, book); CHECK(book->count == 0U && book->revision == 1U);
        CHECK(!gtk_widget_get_sensitive(apply)); goto done;
    }
    if (!strcmp(mode, "capture-nested")) CHECK(probe.actions == 1U);
    if (!strcmp(mode, "cross-currency")) Entry(window, "treasury.accounting.currency", "USD");
    if (strcmp(mode, "unbalanced")) Credit(window);
    Read(window, book); CHECK(book->count == (!strcmp(mode, "unbalanced") ? 1U : 2U));
    if (!strcmp(mode, "capture-pair") || !strcmp(mode, "capture-nested")) goto done;
    if (!strcmp(mode, "invalid-api")) {
        *before = *book;
        CHECK(UmiGtk4AccountingPracticeRead(NULL, book) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4AccountingPracticeRead(window, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4AccountingPracticeRead(parent, book) == UMI_STATUS_INVALID_STATE);
        CHECK(!memcmp(before, book, sizeof *book)); goto done;
    }
    if (!strcmp(mode, "parent")) {
        *before = *book; gtk_window_destroy(parent);
        g_signal_emit_by_name(capture, "clicked"); g_signal_emit_by_name(apply, "clicked");
        CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
        CHECK(!memcmp(before, book, sizeof *book));
        CHECK(UmiGtk4AccountingPracticeCreate(parent) == NULL); goto done;
    }
    if (!strcmp(mode, "edit-preview") || !strcmp(mode, "close-preview"))
        g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.accounting.preview"))),
            "changed", G_CALLBACK(Preview), &probe);
    if (!strcmp(mode, "invalid-action"))
        gtk_drop_down_set_model(GTK_DROP_DOWN(Find(window, "treasury.accounting.action")), NULL);
    Click(window, "treasury.accounting.prepare");
    if (!strcmp(mode, "close-preview")) {
        CHECK(probe.actions == 1U);
        *before = *book; g_signal_emit_by_name(apply, "clicked");
        CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
        CHECK(!memcmp(before, book, sizeof *book)); goto done;
    }
    if (!strcmp(mode, "edit-preview")) CHECK(probe.actions == 1U);
    if (!strcmp(mode, "changed-form")) Entry(window, "treasury.accounting.reference", "later");
    if (!strcmp(mode, "changed-action"))
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "treasury.accounting.action")), 1U);
    if (!strcmp(mode, "ack-edit") || !strcmp(mode, "ack-click"))
        g_signal_connect(apply, "notify::sensitive", G_CALLBACK(Sensitive), &probe);
    if (strcmp(mode, "unacknowledged")) Accept(window);
    Read(window, book); CHECK(!book->approved && !book->posted);
    gboolean refused = !strcmp(mode, "unbalanced") || !strcmp(mode, "cross-currency") ||
        !strcmp(mode, "unacknowledged") || !strcmp(mode, "changed-form") ||
        !strcmp(mode, "changed-action") || !strcmp(mode, "edit-preview") ||
        !strcmp(mode, "invalid-action") || !strcmp(mode, "ack-edit");
    CHECK(gtk_widget_get_sensitive(apply) == !refused);
    *before = *book; g_signal_emit_by_name(apply, "clicked"); Read(window, book);
    if (refused) CHECK(!memcmp(before, book, sizeof *book));
    else {
        CHECK(book->approved && !book->posted && book->revision == before->revision + 1U);
        *before = *book; g_signal_emit_by_name(apply, "clicked"); Read(window, book);
        CHECK(!memcmp(before, book, sizeof *book));
        if (!strcmp(mode, "repeat")) {
            Click(window, "treasury.accounting.prepare"); Accept(window); g_signal_emit_by_name(apply, "clicked");
            Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
        }
        if (!strcmp(mode, "edit-approved")) {
            Click(window, "treasury.accounting.capture"); Read(window, book);
            CHECK(book->count == 3U && !book->approved && !book->posted);
        }
        if (!strcmp(mode, "post") || !strcmp(mode, "posted-capture")) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "treasury.accounting.action")), 1U);
            Click(window, "treasury.accounting.prepare"); Accept(window); g_signal_emit_by_name(apply, "clicked");
            Read(window, book); CHECK(book->approved && book->posted && book->revision == before->revision + 1U);
            if (!strcmp(mode, "posted-capture")) {
                *before = *book; Click(window, "treasury.accounting.capture"); Read(window, book);
                CHECK(!memcmp(before, book, sizeof *book));
            }
        }
    }
done:
    gtk_window_destroy(window); gtk_window_destroy(parent);
    g_signal_emit_by_name(capture, "clicked"); g_signal_emit_by_name(apply, "clicked");
    *before = *book;
    CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
    CHECK(!memcmp(before, book, sizeof *book));
    g_object_unref(field); g_object_unref(capture); g_object_unref(apply);
    g_object_unref(window); g_object_unref(parent);
    free(before); free(book); return 0;
}
