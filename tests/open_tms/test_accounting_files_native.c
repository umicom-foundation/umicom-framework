/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_accounting_files_native.c
 *
 * PURPOSE:
 *   Exercise accounting file snapshot ownership, review decisions and retired native hosts.
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
#include "umicom/open_tms/accounting_file.h"
#include "../build_log/fixture.h"
static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(widget != NULL); return widget;
}
static void Click(GtkWindow *window, const char *name) { g_signal_emit_by_name(Find(window, name), "clicked"); }
static void Entry(GtkWindow *window, const char *name, const char *text) {
    gtk_editable_set_text(GTK_EDITABLE(Find(window, name)), text);
}
static void Read(GtkWindow *window, UmiOpenTmsAccountingBook *book) {
    CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_OK);
}
static gboolean Ready(GtkWindow *window) {
    bool running = true, ready = false;
    CHECK(UmiGtk4AccountingPracticeFileState(window, &running, &ready) == UMI_STATUS_OK && !running);
    return ready;
}
static void Wait(GtkWindow *window) {
    gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
    for (;;) {
        bool running = true, ready = false;
        CHECK(UmiGtk4AccountingPracticeFileState(window, &running, &ready) == UMI_STATUS_OK);
        if (!running) return;
        CHECK(g_get_monotonic_time() < deadline); g_main_context_iteration(NULL, FALSE); g_usleep(1000);
    }
}
static void Fill(UmiOpenTmsAccountingBook *book) {
    umi_open_tms_accounting_book_init(book);
    UmiOpenTmsAccountingLine row = {0};
    strcpy(row.accountId, "imported-cash"); strcpy(row.referenceId, "synthetic-only");
    strcpy(row.currency, "EUR"); row.debit = 1250.0;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
    strcpy(row.accountId, "imported-receivable"); row.debit = 0.0; row.credit = 1250.0;
    CHECK(umi_open_tms_accounting_book_add(book, &row) == UMI_STATUS_OK);
    CHECK(umi_open_tms_accounting_book_approve(book) == UMI_STATUS_OK);
}
static void Accept(GtkWindow *window) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.accounting.file-ack")), TRUE);
}
typedef struct Probe { GtkWindow *window; GtkWidget *field, *apply; const char *mode; unsigned calls; } Probe;
static void PreviewChanged(GtkTextBuffer *buffer, gpointer value) {
    Probe *probe = value;
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    gboolean review = strstr(text, "LOADED — NOT APPLIED") != NULL; g_free(text);
    if (!review || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "close-preview")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
}
static void ApplyNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec; Probe *probe = value;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "ack-edit")) gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
    else g_signal_emit_by_name(probe->apply, "clicked");
}
static void NoteNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec; Probe *probe = value;
    const char *text = gtk_label_get_text(GTK_LABEL(object));
    if ((strstr(text, "Opening captured") == NULL && strstr(text, "Saving captured") == NULL) ||
        probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "start-close")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
}
int main(int argc, char **argv) {
    CHECK(argc == 2); if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "accounting-caf\xc3\xa9.umiv");
    UmiOpenTmsAccountingBook *loaded = malloc(sizeof *loaded), *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(loaded && book && before); Fill(loaded);
    if (!strcmp(mode, "invalid-utf8")) loaded->lines[0].accountId[0] = (char)0xFF;
    gboolean save = !strcmp(mode, "save") || !strcmp(mode, "save-snapshot") ||
        !strcmp(mode, "save-close") || !strcmp(mode, "start-close");
    if (!save && strcmp(mode, "missing") && strcmp(mode, "malformed") && strcmp(mode, "relative"))
        CHECK(umi_open_tms_accounting_book_save_new(path, loaded, NULL) == UMI_STATUS_OK);
    if (!strcmp(mode, "malformed")) {
        UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, "bad", 3U) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
    }
    GtkWindow *window = UmiGtk4AccountingPracticeCreate(NULL); CHECK(window); g_object_ref(window);
    Click(window, "treasury.accounting.capture");
    Entry(window, "treasury.accounting.account", "current-receivable");
    Entry(window, "treasury.accounting.debit", "0"); Entry(window, "treasury.accounting.credit", "100");
    Click(window, "treasury.accounting.capture"); Read(window, before); CHECK(before->count == 2U);
    Entry(window, "treasury.accounting.path", !strcmp(mode, "relative") ? "relative.umiv" : path);
    GtkWidget *field = Find(window, "treasury.accounting.reference"), *apply = Find(window, "treasury.accounting.file-apply");
    g_object_ref(field); g_object_ref(apply);
    Probe probe = {window, field, apply, mode, 0U};
    if (!strcmp(mode, "edit-preview") || !strcmp(mode, "close-preview"))
        g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.accounting.file-preview"))),
            "changed", G_CALLBACK(PreviewChanged), &probe);
    if (!strcmp(mode, "start-close") || !strcmp(mode, "start-edit"))
        g_signal_connect(Find(window, "treasury.accounting.note"), "notify::label", G_CALLBACK(NoteNotify), &probe);
    if (!strcmp(mode, "revokes-action")) {
        Click(window, "treasury.accounting.prepare");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.accounting.ack")), TRUE);
        CHECK(gtk_widget_get_sensitive(Find(window, "treasury.accounting.apply")));
    }
    Click(window, save || !strcmp(mode, "existing") ? "treasury.accounting.save" : "treasury.accounting.open");
    if (!strcmp(mode, "start-close")) {
        CHECK(probe.calls == 1U);
        CHECK(umi_open_tms_accounting_book_load(path, book) != UMI_STATUS_OK);
    } else if (!strcmp(mode, "save-close")) {
        gtk_window_destroy(window);
        gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
        while (umi_open_tms_accounting_book_load(path, book) != UMI_STATUS_OK) {
            CHECK(g_get_monotonic_time() < deadline); g_main_context_iteration(NULL, FALSE); g_usleep(1000);
        }
        CHECK(book->count == 2U && !strcmp(book->lines[0].currency, "GBP"));
        CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(mode, "close-preview")) {
        gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
        while (probe.calls == 0U) {
            CHECK(g_get_monotonic_time() < deadline); g_main_context_iteration(NULL, FALSE); g_usleep(1000);
        }
        CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
    } else {
        if (!strcmp(mode, "dirty-load")) Entry(window, "treasury.accounting.reference", "changed");
        if (!strcmp(mode, "save-snapshot")) Click(window, "treasury.accounting.capture");
        Wait(window);
        if (save) {
            CHECK(umi_open_tms_accounting_book_load(path, book) == UMI_STATUS_OK);
            CHECK(book->count == 2U && book->lines[0].debit == 100.0 && !book->approved);
            CHECK(!Ready(window));
            if (!strcmp(mode, "save-snapshot")) { Read(window, book); CHECK(book->count == 3U); }
        } else if (!strcmp(mode, "existing")) {
            CHECK(umi_open_tms_accounting_book_load(path, book) == UMI_STATUS_OK);
            CHECK(book->approved && !strcmp(book->lines[0].currency, "EUR")); CHECK(!Ready(window));
        } else if (!strcmp(mode, "missing") || !strcmp(mode, "relative") || !strcmp(mode, "malformed") ||
            !strcmp(mode, "invalid-utf8") || !strcmp(mode, "dirty-load") || !strcmp(mode, "edit-preview") || !strcmp(mode, "start-edit")) {
            CHECK(!Ready(window)); Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
            CHECK(!gtk_widget_get_sensitive(apply));
        } else {
            CHECK(Ready(window)); Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
            CHECK(!gtk_widget_get_sensitive(apply));
            if (!strcmp(mode, "review")) {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.accounting.file-preview")));
                GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
                char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
                CHECK(strstr(text, "CURRENT") && strstr(text, "LOADED — NOT APPLIED") &&
                    strstr(text, "Approved yes") && strstr(text, "1250") && strstr(text, "EUR") &&
                    strstr(text, "synthetic-only") && strstr(text, "imported-receivable")); g_free(text);
            } else if (!strcmp(mode, "unacknowledged")) {
                Click(window, "treasury.accounting.file-apply"); Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
            } else if (!strcmp(mode, "edited-review")) {
                Entry(window, "treasury.accounting.path", "other"); Accept(window);
                Click(window, "treasury.accounting.file-apply"); Read(window, book);
                CHECK(!Ready(window) && !memcmp(before, book, sizeof *book));
            } else if (!strcmp(mode, "ack-edit") || !strcmp(mode, "ack-click")) {
                g_signal_connect(apply, "notify::sensitive", G_CALLBACK(ApplyNotify), &probe);
                Accept(window); Read(window, book); CHECK(!memcmp(before, book, sizeof *book) && probe.calls == 1U);
                if (!strcmp(mode, "ack-edit")) CHECK(!Ready(window) && !gtk_widget_get_sensitive(apply));
                else { CHECK(Ready(window)); Click(window, "treasury.accounting.file-apply"); Read(window, book);
                    CHECK(book->approved && !strcmp(book->lines[0].currency, "EUR")); }
            } else if (!strcmp(mode, "close-pending")) {
                Accept(window); gtk_window_destroy(window); g_signal_emit_by_name(apply, "clicked");
                CHECK(UmiGtk4AccountingPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
            } else if (!strcmp(mode, "apply") || !strcmp(mode, "replay") || !strcmp(mode, "revokes-action")) {
                if (!strcmp(mode, "revokes-action")) {
                    CHECK(!gtk_widget_get_sensitive(Find(window, "treasury.accounting.apply")));
                    Click(window, "treasury.accounting.apply"); Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
                }
                Accept(window); Click(window, "treasury.accounting.file-apply"); Read(window, book);
                CHECK(book->count == 2U && book->approved && !book->posted && !strcmp(book->lines[0].currency, "EUR"));
                CHECK(!Ready(window));
                if (!strcmp(mode, "replay")) {
                    *before = *book; g_signal_emit_by_name(apply, "clicked"); Read(window, book); CHECK(!memcmp(before, book, sizeof *book));
                }
            } else if (!strcmp(mode, "invalid-api")) {
                bool a = true, b = false;
                CHECK(UmiGtk4AccountingPracticeFileState(NULL, &a, &b) == UMI_STATUS_INVALID_ARGUMENT && a && !b);
                CHECK(UmiGtk4AccountingPracticeFileState(window, &a, &a) == UMI_STATUS_INVALID_ARGUMENT && a);
                gtk_window_destroy(window);
                CHECK(UmiGtk4AccountingPracticeFileState(window, &a, &b) == UMI_STATUS_INVALID_STATE && a && !b);
            } else return 2;
        }
    }
    /* Retained controls are released before the stack observer expires. Late
     * completion owns its capture and refuses all access to the retired host. */
    gtk_window_destroy(window); g_object_unref(apply); g_object_unref(field); g_object_unref(window);
    free(before); free(book); free(loaded); return 0;
}
