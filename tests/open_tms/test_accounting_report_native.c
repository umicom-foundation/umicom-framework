/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_report_native.c
 *
 * PURPOSE:
 *   Check immutable native accounting reports, explicit export and retained-window retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/accounting_report.h"
#include "umicom/ui/gtk4/accounting_practice.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/open_tms/accounting_report.h"
#include "../build_log/fixture.h"
#include "../support/gtk4_expanders.h"

static GtkWidget *Find(GtkWidget *root, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(root, name);
    CHECK(widget); return widget;
}
static void Fill(UmiOpenTmsAccountingBook *book) {
    umi_open_tms_accounting_book_init(book);
    UmiOpenTmsAccountingLine row = {0};
    strcpy(row.accountId, "practice"); strcpy(row.referenceId, "=example");
    strcpy(row.currency, "GBP"); row.debit = 100.0;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
    row.debit = 0.0; row.credit = 100.0;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
}
static void Wait(GtkWidget *export) {
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiTextExportGtk4Busy(export) && g_get_monotonic_time() < deadline) {
        /* Bounded dispatch prevents an always-ready GUI source hiding the deadline. */
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i)
            g_main_context_iteration(NULL, FALSE);
        g_usleep(1000);
    }
    CHECK(!UmiTextExportGtk4Busy(export));
}
static char *Text(GtkWidget *view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
int main(int argc, char **argv) {
    const char *cases[] = {"empty", "balanced", "currencies", "quoted-text", "immutable", "released-source",
        "invalid-utf8", "invalid-approved", "null-book", "null-output", "invalid-parent", "save", "existing",
        "retired", "parent-close", "accepted-close", "launcher"};
    CHECK(argc == 2); int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(cases[i], argv[1])) known = 1;
    CHECK(known);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    UmiOpenTmsAccountingBook *book = malloc(sizeof *book); CHECK(book); Fill(book);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    GtkWindow *window = NULL;
    if (!strcmp(mode, "null-output")) {
        CHECK(UmiGtk4AccountingReportCreate(parent, book, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(mode, "invalid-parent")) {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0); g_object_ref_sink(box);
        CHECK(UmiGtk4AccountingReportCreate((GtkWindow *)box, book, &window) == UMI_STATUS_INVALID_ARGUMENT && !window);
        g_object_unref(box);
    } else if (!strcmp(mode, "invalid-utf8") || !strcmp(mode, "invalid-approved") || !strcmp(mode, "null-book")) {
        if (!strcmp(mode, "invalid-utf8")) { book->lines[1].accountId[0] = (char)0xff; book->lines[1].accountId[1] = '\0'; }
        if (!strcmp(mode, "invalid-approved")) { book->approved = 1; book->lines[1].credit = 1.0; }
        CHECK(UmiGtk4AccountingReportCreate(parent, !strcmp(mode, "null-book") ? NULL : book, &window) != UMI_STATUS_OK && !window);
    } else if (!strcmp(mode, "launcher")) {
        GtkWindow *practice = UmiGtk4AccountingPracticeCreate(parent); CHECK(practice); g_object_ref(practice);
        g_signal_emit_by_name(Find(GTK_WIDGET(practice), "treasury.accounting.report"), "clicked");
        GListModel *windows = gtk_window_get_toplevels(); unsigned matched = 0U;
        for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
            GtkWindow *candidate = g_list_model_get_item(windows, i);
            if (!strcmp(gtk_widget_get_name(GTK_WIDGET(candidate)), "treasury.accounting.report.window")) {
                ++matched; CHECK(gtk_window_get_transient_for(candidate) == practice);
            }
            g_object_unref(candidate);
        }
        CHECK(matched == 1U); gtk_window_destroy(practice); g_object_unref(practice);
    } else {
        if (!strcmp(mode, "empty")) umi_open_tms_accounting_book_init(book);
        if (!strcmp(mode, "currencies")) strcpy(book->lines[1].currency, "USD");
        if (!strcmp(mode, "quoted-text")) strcpy(book->lines[1].referenceId, "a,\"b\"");
        UmiCsvDocument *expected = NULL;
        CHECK(umi_open_tms_accounting_book_report(book, &expected) == UMI_STATUS_OK);
        CHECK(UmiGtk4AccountingReportCreate(parent, book, &window) == UMI_STATUS_OK && window);
        g_object_ref(window);
        GtkWidget *view = Find(GTK_WIDGET(window), "treasury.accounting.report.csv");
        CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(view)));
        char *text = Text(view); CHECK(!strcmp(text, UmiCsvDocumentData(expected)));
        g_free(text);
        if (!strcmp(mode, "immutable")) book->lines[0].debit = 42.0;
        if (!strcmp(mode, "released-source")) { free(book); book = NULL; }
        text = Text(view); CHECK(!strcmp(text, UmiCsvDocumentData(expected))); g_free(text);
        GtkWidget *export = g_object_ref(Find(GTK_WIDGET(window), "treasury.accounting.report.export"));
        /* The export expander retains its controls outside the widget tree until opened. */
        /* The export is a composite control. Keep the direct draft cast for
         * review; the shared fixture opens only verified expander instances. */
#if 0
        gtk_expander_set_expanded(GTK_EXPANDER(export), TRUE);
#endif
        UmiTestExpandSections(export);
        GtkWidget *save = g_object_ref(Find(export, "text.export.save"));
        GtkWidget *entry = Find(export, "text.export.path");
        if (!strcmp(mode, "save") || !strcmp(mode, "existing") || !strcmp(mode, "retired") ||
            !strcmp(mode, "parent-close") || !strcmp(mode, "accepted-close")) {
            char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY]; FixtureDirectory(root);
            FixturePath(path, root, "accounting-report.csv"); gtk_editable_set_text(GTK_EDITABLE(entry), path);
            if (!strcmp(mode, "existing")) {
                UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
                CHECK(UmiOutputFileWrite(file, "keep", 4U) == UMI_STATUS_OK && UmiOutputFileClose(file) == UMI_STATUS_OK);
                UmiOutputFileDestroy(file);
            }
            if (!strcmp(mode, "retired")) gtk_window_destroy(window);
            if (!strcmp(mode, "parent-close")) gtk_window_destroy(parent);
            g_signal_emit_by_name(save, "clicked");
            if (!strcmp(mode, "accepted-close")) {
                CHECK(UmiTextExportGtk4Busy(export)); gtk_window_destroy(window);
            }
            Wait(export);
            if (!strcmp(mode, "retired") || !strcmp(mode, "parent-close")) {
                CHECK(!gtk_widget_get_sensitive(save));
                UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
                CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
            } else {
                size_t size = 0U; unsigned char *bytes = FixtureRead(path, &size);
                const char *wanted = !strcmp(mode, "existing") ? "keep" : UmiCsvDocumentData(expected);
                CHECK(size == strlen(wanted) && memcmp(bytes, wanted, size) == 0); free(bytes);
            }
        }
        gtk_window_destroy(window); g_object_unref(window);
        g_object_unref(save); g_object_unref(export); UmiCsvDocumentDestroy(expected);
    }
    gtk_window_destroy(parent); g_object_unref(parent); free(book); return 0;
}
