/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_archive_workflow.c
 *
 * PURPOSE:
 *   Exercise actual asynchronous saved-review reopening and request retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "../support/main_context.h"
#include "umicom/bank_feed/statement_archive.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <glib/gstdio.h>
#undef CHECK
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static bool Wait(GtkWindow *window) {
    gint64 deadline = g_get_monotonic_time() + 10000000;
    while (UmiBankFeedGtk4Busy(window) && g_get_monotonic_time() < deadline) {
        (void)UmiTestDispatchSlice(NULL, deadline, 32U);
        g_usleep(1000U);
    }
    return !UmiBankFeedGtk4Busy(window);
}
/* Return an owned reference to the actual child produced by async completion,
 * rather than manufacturing a second window through the direct constructor. */
static GtkWindow *ReviewWindow(GtkWindow *parent) {
    GListModel *windows = gtk_window_get_toplevels();
    GtkWindow *found = NULL;
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent && Find(window, "statement-review-export") != NULL) {
            CHECK(found == NULL);
            found = window;
        } else g_object_unref(window);
    }
    return found;
}
typedef struct Probe { GtkWindow *window; GtkWidget *path; bool close; unsigned calls; } Probe;
static void Publish(GObject *object, GParamSpec *spec, gpointer data) {
    (void)spec; Probe *probe = data;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->path), "changed-before-review-window");
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char *source = BankFixture(BANK_TEST_ROW);
    CHECK(source != NULL);
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCreate(source, strlen(source), &feed) == UMI_STATUS_OK);
    free(source);
    UmiBankStatementRequest request = {0};
    request.account = "practice-account"; request.currency = "GBP";
    CHECK(UmiBankStatementDate("2026-10-12", &request.firstDate) == UMI_STATUS_OK);
    request.lastDate = request.firstDate;
    request.openingBalance = (UmiDecimal){10000, 2U};
    request.closingBalance = (UmiDecimal){11250, 2U};
    UmiBankFeedArchiveInfo info = {.sourceFormat = UMI_BANK_FEED_SOURCE_MAPPED_CSV};
    memset(info.sourceSha256, 'c', 64U);
    UmiBankStatementArchive *archive = NULL;
    CHECK(UmiBankStatementArchiveCreate(feed, &request, &info, &archive) == UMI_STATUS_OK);
    UmiBankFeedDestroy(feed);
    char *bytes = NULL; size_t length = 0U;
    CHECK(UmiBankStatementArchiveEncode(archive, &bytes, &length) == UMI_STATUS_OK);
    UmiBankStatementArchiveDestroy(archive);
    GError *error = NULL;
    char *directory = g_dir_make_tmp("umicom-statement-reopen-XXXXXX", &error);
    CHECK(directory && error == NULL);
    char *filename = g_build_filename(directory, "review.json", NULL);
    const char *contents = !strcmp(mode, "invalid") ? "{\"format\":\"wrong\"}" : bytes;
    CHECK(g_file_set_contents(filename, contents, -1, &error));
    UmiBankFeedArchiveFree(bytes);
    GtkWindow *window = UmiBankFeedGtk4Create(NULL); CHECK(window); g_object_ref(window);
    GtkWidget *format = Find(window, "bank-feed-format"), *path = Find(window, "bank-feed-path"),
        *load = Find(window, "bank-feed-load"), *output = Find(window, "bank-feed-output"),
        *save = Find(window, "bank-feed-archive-export");
    CHECK(format && path && load && output && save);
    g_object_ref(load); g_object_ref(save); g_object_ref(output);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 4U);
    gtk_editable_set_text(GTK_EDITABLE(path), filename);
    Probe probe = {window, path, !strcmp(mode, "close-notify"), 0U};
    bool notify = !strcmp(mode, "edit-notify") || !strcmp(mode, "close-notify");
    gulong signal = notify ? g_signal_connect(save, "notify::sensitive", G_CALLBACK(Publish), &probe) : 0U;
    g_signal_emit_by_name(load, "clicked");
    bool refuse = notify || !strcmp(mode, "invalid") || !strcmp(mode, "format-change") ||
        !strcmp(mode, "path-change") || !strcmp(mode, "close-loading");
    if (!strcmp(mode, "format-change")) gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
    if (!strcmp(mode, "path-change")) gtk_editable_set_text(GTK_EDITABLE(path), "changed-after-request");
    if (!strcmp(mode, "close-loading")) gtk_window_destroy(window);
    CHECK(Wait(window));
    GtkWindow *child = ReviewWindow(window);
    if (refuse) {
        CHECK(child == NULL);
        if (notify) CHECK(probe.calls > 0U);
        CHECK(!gtk_widget_get_sensitive(save));
    } else {
        CHECK(child != NULL);
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(child, "statement-opening"))), "100.00"));
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(child, "statement-closing"))), "112.50"));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(child, "statement-result"))), "Arithmetic matches"));
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output));
        GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
        char *report = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
        CHECK(strstr(report, info.sourceSha256)); g_free(report);
        if (!strcmp(mode, "independent")) {
            gtk_editable_set_text(GTK_EDITABLE(path), "another-file");
            CHECK(gtk_widget_get_sensitive(Find(child, "statement-review-export")));
        } else CHECK(!strcmp(mode, "roundtrip") || !strcmp(mode, "parent"));
        GtkWidget *retained = Find(child, "statement-review-export"); g_object_ref(retained);
        gtk_window_destroy(window);
        CHECK(!gtk_widget_get_sensitive(retained));
        g_object_unref(retained); gtk_window_destroy(child); g_object_unref(child);
    }
    if (signal) g_signal_handler_disconnect(save, signal);
    gtk_window_destroy(window); CHECK(Wait(window));
    g_signal_emit_by_name(load, "clicked");
    CHECK(!UmiBankFeedGtk4Busy(window) && ReviewWindow(window) == NULL);
    CHECK(g_remove(filename) == 0 && g_rmdir(directory) == 0);
    g_free(filename); g_free(directory); g_clear_error(&error);
    g_object_unref(load); g_object_unref(save); g_object_unref(output); g_object_unref(window);
    return 0;
}
