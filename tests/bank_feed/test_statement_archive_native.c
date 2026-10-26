/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_statement_archive_native.c
 *
 * PURPOSE:
 *   Exercise private review saves, reopened assumptions and native invalidation boundaries.
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
#include "umicom/ui/gtk4/bank_statement.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
#undef CHECK
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static GtkWidget *Tagged(GtkWidget *root, const char *name) {
    return umi_gtk4_automation_find_tagged_widget(root, name);
}
static bool Wait(GtkWidget *save) {
    gint64 deadline = g_get_monotonic_time() + 10000000;
    while (UmiTextExportGtk4Busy(save) && g_get_monotonic_time() < deadline) {
        (void)UmiTestDispatchSlice(NULL, deadline, 32U);
        g_usleep(1000U);
    }
    return !UmiTextExportGtk4Busy(save);
}
typedef struct Probe {
    GtkWindow *window;
    GtkWidget *closing, *compare;
    const char *mode;
    unsigned calls;
} Probe;
static void Published(GObject *object, GParamSpec *spec, gpointer data) {
    (void)spec;
    Probe *probe = data;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++) return;
    if (!strcmp(probe->mode, "close-notify")) gtk_window_destroy(probe->window);
    else if (!strcmp(probe->mode, "edit-notify"))
        gtk_editable_set_text(GTK_EDITABLE(probe->closing), "999");
    else if (!strcmp(probe->mode, "nested-compare"))
        g_signal_emit_by_name(probe->compare, "clicked");
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    char *input = BankFixture(BANK_TEST_ROW);
    CHECK(input != NULL);
    UmiBankFeed *feed = NULL;
    CHECK(UmiBankFeedCreate(input, strlen(input), &feed) == UMI_STATUS_OK);
    free(input);
    UmiBankFeedArchiveInfo info = {.sourceFormat = UMI_BANK_FEED_SOURCE_OPEN_BANKING_JSON};
    memset(info.sourceSha256, 'b', 64U);
    GtkWindow *parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
    GtkWindow *window = !strcmp(mode, "legacy")
        ? UmiBankStatementGtk4Create(feed, "practice-account", "GBP", parent)
        : UmiBankStatementGtk4CreateWithSource(feed, "practice-account", "GBP", &info, parent);
    CHECK(window != NULL); g_object_ref(window);
    if (!strcmp(mode, "invalid-source")) {
        UmiBankFeedArchiveInfo invalid = info;
        invalid.sourceFormat = (UmiBankFeedSourceFormat)99;
        CHECK(UmiBankStatementGtk4CreateWithSource(feed, "practice-account", "GBP", &invalid, parent) == NULL);
        memset(invalid.sourceSha256, 'b', sizeof invalid.sourceSha256);
        invalid.sourceFormat = UMI_BANK_FEED_SOURCE_OPEN_BANKING_JSON;
        CHECK(UmiBankStatementGtk4CreateWithSource(feed, "practice-account", "GBP", &invalid, parent) == NULL);
        CHECK(UmiBankFeedArchiveInfoValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankFeedArchiveInfoValidate(&info) == UMI_STATUS_OK);
    }
    UmiBankFeedDestroy(feed);
    GtkWidget *first = Find(window, "statement-first"), *last = Find(window, "statement-last"),
        *opening = Find(window, "statement-opening"), *closing = Find(window, "statement-closing"),
        *compare = Find(window, "statement-compare"), *result = Find(window, "statement-result"),
        *save = Find(window, "statement-review-export");
    CHECK(first && last && opening && closing && compare && result && save);
    g_object_ref(compare); g_object_ref(result); g_object_ref(save);
    GtkWidget *savePath = Tagged(save, "text.export.path"), *saveButton = Tagged(save, "text.export.save");
    CHECK(savePath && saveButton); g_object_ref(saveButton);
    CHECK(!gtk_widget_get_sensitive(save));
    gtk_editable_set_text(GTK_EDITABLE(first), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(last), "2026-10-12");
    gtk_editable_set_text(GTK_EDITABLE(opening), "100.00");
    gtk_editable_set_text(GTK_EDITABLE(closing), !strcmp(mode, "mismatch") ? "113.50" : "112.50");
    GError *error = NULL;
    char *directory = g_dir_make_tmp("umicom-statement-review-XXXXXX", &error);
    CHECK(directory != NULL && error == NULL);
    char *destination = g_build_filename(directory, "review.json", NULL);
    gtk_editable_set_text(GTK_EDITABLE(savePath), destination);
    Probe probe = {window, closing, compare, mode, 0U};
    bool notify = !strcmp(mode, "edit-notify") || !strcmp(mode, "close-notify") || !strcmp(mode, "nested-compare");
    gulong signal = notify ? g_signal_connect(save, "notify::sensitive", G_CALLBACK(Published), &probe) : 0U;
    if (!strcmp(mode, "invalid")) gtk_editable_set_text(GTK_EDITABLE(first), "2026-02-30");
    g_signal_emit_by_name(compare, "clicked");
    bool refused = !strcmp(mode, "invalid") || !strcmp(mode, "edit-notify") || !strcmp(mode, "close-notify");
    if (!refused) CHECK(gtk_widget_get_sensitive(save));
    if (notify) CHECK(probe.calls > 0U);
    if (!strcmp(mode, "edit")) {
        gtk_editable_set_text(GTK_EDITABLE(closing), "113.50");
        refused = true;
    } else if (!strcmp(mode, "retained")) {
        gtk_window_destroy(window);
        refused = true;
    } else if (!strcmp(mode, "parent")) {
        gtk_window_destroy(parent);
        refused = true;
    }
    if (refused) CHECK(!gtk_widget_get_sensitive(save));
    g_signal_emit_by_name(saveButton, "clicked");
    CHECK(Wait(save));
    if (refused) {
        CHECK(!g_file_test(destination, G_FILE_TEST_EXISTS));
    } else {
        char *saved = NULL; gsize length = 0U;
        CHECK(g_file_get_contents(destination, &saved, &length, &error));
        UmiBankStatementArchive *archive = NULL;
        CHECK(UmiBankStatementArchiveDecode(saved, length, &archive) == UMI_STATUS_OK);
        UmiBankStatementArchiveSnapshot snapshot;
        CHECK(UmiBankStatementArchiveRead(archive, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.result.balancesMatch == (strcmp(mode, "mismatch") != 0));
        CHECK(snapshot.source.sourceFormat == (!strcmp(mode, "legacy") ? UMI_BANK_FEED_SOURCE_UNKNOWN : info.sourceFormat));
        CHECK(!strcmp(snapshot.source.sourceSha256, !strcmp(mode, "legacy") ? "" : info.sourceSha256));
        if (!strcmp(mode, "exclusive")) {
            g_signal_emit_by_name(saveButton, "clicked"); CHECK(Wait(save));
            char *same = NULL; gsize sameLength = 0U;
            CHECK(g_file_get_contents(destination, &same, &sameLength, &error));
            CHECK(sameLength == length && memcmp(saved, same, length) == 0); g_free(same);
            GtkWidget *status = Tagged(save, "text.export.status");
            CHECK(status && strstr(gtk_label_get_text(GTK_LABEL(status)), "Save failed"));
        }
        GtkWindow *reopened = UmiBankStatementGtk4CreateFromArchive(archive, parent);
        CHECK(reopened != NULL); g_object_ref(reopened);
        UmiBankStatementArchiveDestroy(archive);
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(reopened, "statement-first"))), "2026-10-12"));
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(reopened, "statement-opening"))), "100.00"));
        CHECK(gtk_widget_get_sensitive(Find(reopened, "statement-review-export")));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(reopened, "statement-result"))),
            !strcmp(mode, "mismatch") ? "differs" : "Arithmetic matches"));
        gtk_window_destroy(reopened); g_object_unref(reopened);
        g_free(saved); CHECK(g_remove(destination) == 0);
        CHECK(!strcmp(mode, "roundtrip") || !strcmp(mode, "mismatch") || !strcmp(mode, "legacy") ||
            !strcmp(mode, "nested-compare") || !strcmp(mode, "invalid-source") || !strcmp(mode, "exclusive"));
    }
    if (signal != 0U) g_signal_handler_disconnect(save, signal);
    gtk_window_destroy(window); gtk_window_destroy(parent);
    g_signal_emit_by_name(compare, "clicked");
    g_signal_emit_by_name(saveButton, "clicked"); CHECK(Wait(save));
    CHECK(!g_file_test(destination, G_FILE_TEST_EXISTS));
    CHECK(UmiBankStatementGtk4CreateFromArchive(NULL, NULL) == NULL);
    CHECK(g_rmdir(directory) == 0);
    g_free(destination); g_free(directory); g_clear_error(&error);
    g_object_unref(saveButton); g_object_unref(compare); g_object_unref(result); g_object_unref(save);
    g_object_unref(window); g_object_unref(parent);
    return 0;
}
