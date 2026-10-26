/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_forward_files_native.c
 *
 * PURPOSE:
 *   Exercise asynchronous treasury file ownership, review invalidation and host retirement.
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
#include "umicom/open_tms/fx_forward_file.h"
#include "../build_log/fixture.h"

static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(widget != NULL); return widget;
}
static void Click(GtkWindow *window, const char *name) {
    GtkWidget *button = Find(window, name); CHECK(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
}
static void Entry(GtkWindow *window, const char *name, const char *text) {
    gtk_editable_set_text(GTK_EDITABLE(Find(window, name)), text);
}
static gboolean Ready(GtkWindow *window) {
    bool running = true, ready = false;
    CHECK(UmiGtk4ForwardPracticeFileState(window, &running, &ready) == UMI_STATUS_OK);
    CHECK(!running); return ready;
}
static void Wait(GtkWindow *window) {
    gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
    for (;;) {
        bool running = true, ready = false;
        CHECK(UmiGtk4ForwardPracticeFileState(window, &running, &ready) == UMI_STATUS_OK);
        if (!running) return;
        CHECK(g_get_monotonic_time() < deadline);
        g_main_context_iteration(NULL, FALSE); g_usleep(1000);
    }
}
static void Fill(UmiOpenTmsFxForwardBook *book) {
    umi_open_tms_fx_forward_book_init(book);
    UmiOpenTmsFxForward row = {0};
    strcpy(row.tradeId, "loaded"); strcpy(row.accountId, "practice"); strcpy(row.counterpartyId, "synthetic");
    strcpy(row.baseCurrency, "EUR"); strcpy(row.quoteCurrency, "GBP");
    row.baseNotional = 1250.0; row.forwardRate = 0.85; row.version = 7U;
    row.maturityMilliseconds = UINT64_C(123456789);
    row.state = UMI_OPEN_TMS_FX_FORWARD_DRAFT;
    CHECK(umi_open_tms_fx_forward_book_add(book, &row) == UMI_STATUS_OK);
}
static void Read(GtkWindow *window, UmiOpenTmsFxForwardBook *book) {
    CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_OK);
}
static void Accept(GtkWindow *window) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.forward.file-ack")), TRUE);
}
typedef struct Probe { GtkWindow *window; GtkWidget *field, *apply; const char *mode; unsigned calls; } Probe;
static void PreviewChanged(GtkTextBuffer *buffer, gpointer value) {
    Probe *probe = value;
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    gboolean review = strstr(text, "LOADED — NOT APPLIED") != NULL; g_free(text);
    if (!review || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "close-preview")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
}
static void ApplyNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec;
    Probe *probe = value;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "ack-edit")) gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
    else g_signal_emit_by_name(probe->apply, "clicked");
}
static void NoteNotify(GObject *object, GParamSpec *spec, gpointer value) {
    (void)spec;
    Probe *probe = value;
    const char *text = gtk_label_get_text(GTK_LABEL(object));
    if ((strstr(text, "Opening captured") == NULL && strstr(text, "Saving captured") == NULL) ||
        probe->calls++ != 0U) return;
    if (!strcmp(probe->mode, "start-close")) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->field), "changed");
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "treasury-caf\xc3\xa9.umiv");
    UmiOpenTmsFxForwardBook *loaded = malloc(sizeof *loaded), *book = malloc(sizeof *book), *before = malloc(sizeof *before);
    CHECK(loaded && book && before); Fill(loaded);
    if (!strcmp(mode, "invalid-utf8")) { loaded->trades[0].tradeId[0] = (char)0xFF; }
    gboolean save = !strcmp(mode, "save") || !strcmp(mode, "save-snapshot") ||
        !strcmp(mode, "save-close") || !strcmp(mode, "start-close");
    if (!save && strcmp(mode, "missing") && strcmp(mode, "malformed") && strcmp(mode, "relative"))
        CHECK(umi_open_tms_fx_forward_book_save_new(path, loaded, NULL) == UMI_STATUS_OK);
    if (!strcmp(mode, "malformed")) {
        UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, "bad", 3U) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
    }
    GtkWindow *window = UmiGtk4ForwardPracticeCreate(NULL); CHECK(window); g_object_ref(window);
    Entry(window, "treasury.forward.id", "current"); Click(window, "treasury.forward.capture"); Read(window, before);
    Entry(window, "treasury.forward.path", !strcmp(mode, "relative") ? "relative.umiv" : path);
    GtkWidget *selected = Find(window, "treasury.forward.selected"); g_object_ref(selected);
    GtkWidget *apply = Find(window, "treasury.forward.file-apply"); g_object_ref(apply);
    Probe probe = {window, selected, apply, mode, 0U};
    if (!strcmp(mode, "edit-preview") || !strcmp(mode, "close-preview"))
        g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.forward.file-preview"))),
            "changed", G_CALLBACK(PreviewChanged), &probe);
    if (!strcmp(mode, "start-close") || !strcmp(mode, "start-edit"))
        g_signal_connect(Find(window, "treasury.forward.note"), "notify::label", G_CALLBACK(NoteNotify), &probe);
    if (!strcmp(mode, "revokes-transition")) {
        Click(window, "treasury.forward.prepare");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(window, "treasury.forward.ack")), TRUE);
        CHECK(gtk_widget_get_sensitive(Find(window, "treasury.forward.apply")));
    }
    Click(window, save || !strcmp(mode, "existing") ? "treasury.forward.save" : "treasury.forward.open");
    if (!strcmp(mode, "start-close")) {
        CHECK(probe.calls == 1U);
        CHECK(umi_open_tms_fx_forward_book_load(path, book) != UMI_STATUS_OK);
    } else if (!strcmp(mode, "save-close")) {
        gtk_window_destroy(window);
        gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
        while (umi_open_tms_fx_forward_book_load(path, book) != UMI_STATUS_OK) {
            CHECK(g_get_monotonic_time() < deadline); g_main_context_iteration(NULL, FALSE); g_usleep(1000);
        }
        CHECK(book->count == 1U && !strcmp(book->trades[0].tradeId, "current"));
        CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(mode, "close-preview")) {
        gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
        while (probe.calls == 0U) {
            CHECK(g_get_monotonic_time() < deadline); g_main_context_iteration(NULL, FALSE); g_usleep(1000);
        }
        CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
    } else {
        if (!strcmp(mode, "dirty-load")) Entry(window, "treasury.forward.selected", "changed");
        if (!strcmp(mode, "save-snapshot")) { Entry(window, "treasury.forward.id", "later"); Click(window, "treasury.forward.capture"); }
        Wait(window);
        if (save) {
            CHECK(umi_open_tms_fx_forward_book_load(path, book) == UMI_STATUS_OK);
            CHECK(book->count == 1U && !strcmp(book->trades[0].tradeId, "current"));
            CHECK(!Ready(window));
        } else if (!strcmp(mode, "existing")) {
            CHECK(umi_open_tms_fx_forward_book_load(path, book) == UMI_STATUS_OK);
            CHECK(!strcmp(book->trades[0].tradeId, "loaded")); CHECK(!Ready(window));
        } else if (!strcmp(mode, "missing") || !strcmp(mode, "relative") || !strcmp(mode, "malformed") ||
            !strcmp(mode, "invalid-utf8") || !strcmp(mode, "dirty-load") || !strcmp(mode, "edit-preview") || !strcmp(mode, "start-edit")) {
            CHECK(!Ready(window)); Read(window, book); CHECK(memcmp(before, book, sizeof *book) == 0);
            CHECK(!gtk_widget_get_sensitive(apply));
        } else {
            CHECK(Ready(window)); Read(window, book); CHECK(memcmp(before, book, sizeof *book) == 0);
            CHECK(!gtk_widget_get_sensitive(apply));
            if (!strcmp(mode, "review")) {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(Find(window, "treasury.forward.file-preview")));
                GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
                char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
                CHECK(strstr(text, "CURRENT") && strstr(text, "LOADED — NOT APPLIED") && strstr(text, "1250") &&
                    strstr(text, "123456789") && strstr(text, "EUR/GBP")); g_free(text);
            } else if (!strcmp(mode, "unacknowledged")) {
                Click(window, "treasury.forward.file-apply"); Read(window, book);
                CHECK(memcmp(before, book, sizeof *book) == 0);
            } else if (!strcmp(mode, "edited-review")) {
                Entry(window, "treasury.forward.path", "other"); Accept(window);
                Click(window, "treasury.forward.file-apply"); Read(window, book);
                CHECK(!Ready(window) && memcmp(before, book, sizeof *book) == 0);
            } else if (!strcmp(mode, "ack-edit") || !strcmp(mode, "ack-click")) {
                g_signal_connect(apply, "notify::sensitive", G_CALLBACK(ApplyNotify), &probe);
                Accept(window); Read(window, book); CHECK(memcmp(before, book, sizeof *book) == 0);
                CHECK(probe.calls == 1U);
                if (!strcmp(mode, "ack-edit")) CHECK(!Ready(window) && !gtk_widget_get_sensitive(apply));
                else { CHECK(Ready(window)); Click(window, "treasury.forward.file-apply"); Read(window, book);
                    CHECK(!strcmp(book->trades[0].tradeId, "loaded")); }
            } else if (!strcmp(mode, "close-pending")) {
                Accept(window); gtk_window_destroy(window); g_signal_emit_by_name(apply, "clicked");
                CHECK(UmiGtk4ForwardPracticeRead(window, book) == UMI_STATUS_INVALID_STATE);
            } else if (!strcmp(mode, "apply") || !strcmp(mode, "replay") || !strcmp(mode, "maturity") || !strcmp(mode, "revokes-transition")) {
                if (!strcmp(mode, "revokes-transition")) {
                    CHECK(!gtk_widget_get_sensitive(Find(window, "treasury.forward.apply")));
                    Click(window, "treasury.forward.apply"); Read(window, book);
                    CHECK(memcmp(before, book, sizeof *book) == 0);
                }
                Accept(window); Click(window, "treasury.forward.file-apply"); Read(window, book);
                CHECK(book->count == 1U && !strcmp(book->trades[0].tradeId, "loaded") && book->trades[0].version == 7U);
                CHECK(!Ready(window));
                if (!strcmp(mode, "replay")) {
                    memcpy(before, book, sizeof *before);
                    g_signal_emit_by_name(apply, "clicked"); Read(window, book); CHECK(memcmp(before, book, sizeof *book) == 0);
                }
                if (!strcmp(mode, "maturity")) {
                    Entry(window, "treasury.forward.selected", "loaded"); Click(window, "treasury.forward.prepare");
                    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(window, "treasury.forward.preview"))), "123456789 milliseconds since epoch"));
                }
            } else if (!strcmp(mode, "invalid-api")) {
                bool a = true, b = false;
                CHECK(UmiGtk4ForwardPracticeFileState(NULL, &a, &b) == UMI_STATUS_INVALID_ARGUMENT && a && !b);
                CHECK(UmiGtk4ForwardPracticeFileState(window, &a, &a) == UMI_STATUS_INVALID_ARGUMENT && a);
                gtk_window_destroy(window);
                CHECK(UmiGtk4ForwardPracticeFileState(window, &a, &b) == UMI_STATUS_INVALID_STATE && a && !b);
            } else return 2;
        }
    }
    /* Dispose retained controls before the stack probe expires. A late worker
     * can finish its file, but the retired owner never touches these controls. */
    gtk_window_destroy(window);
    g_object_unref(apply); g_object_unref(selected); g_object_unref(window);
    while (g_main_context_iteration(NULL, FALSE)) {}
    free(before); free(book); free(loaded); return 0;
}
