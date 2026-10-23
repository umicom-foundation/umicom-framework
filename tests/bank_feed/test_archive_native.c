/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_archive_native.c
 * PURPOSE: Exercise real snapshot save/reopen, source identity and native asynchronous retirement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include "csv_fixture.h"
#include "umicom/bank_feed/archive.h"
#include "umicom/base/sha256.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
static GtkWidget *Find(GtkWindow *window, const char *name) {
    return umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
}
static GtkWidget *Tagged(GtkWidget *root, const char *name) {
    return umi_gtk4_automation_find_tagged_widget(root, name);
}
static bool Wait(GtkWindow *window, GtkWidget *export) {
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while ((UmiBankFeedGtk4Busy(window) || UmiTextExportGtk4Busy(export)) &&
        g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiBankFeedGtk4Busy(window) && !UmiTextExportGtk4Busy(export);
}
static char *Text(GtkWidget *view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
}
typedef struct ArchiveProbe {
    GtkWindow *window;
    GtkWidget *path, *format;
    bool close;
    unsigned calls;
} ArchiveProbe;
static void Published(GObject *object, GParamSpec *spec, gpointer data) {
    (void)spec;
    ArchiveProbe *probe = data;
    if (!gtk_widget_get_sensitive(GTK_WIDGET(object)) || probe->calls++) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else gtk_editable_set_text(GTK_EDITABLE(probe->path), "changed-during-publication");
}
static void Prepared(GObject *object, GParamSpec *spec, gpointer data) {
    (void)spec;
    ArchiveProbe *probe = data;
    if (strstr(gtk_label_get_text(GTK_LABEL(object)), "Reading selected file") == NULL || probe->calls++) return;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe->format), 0U);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GtkWindow *window = UmiBankFeedGtk4Create(NULL); CHECK(window != NULL); g_object_ref(window);
    GtkWidget *path = Find(window, "bank-feed-path"), *format = Find(window, "bank-feed-format"),
        *load = Find(window, "bank-feed-load"), *save = Find(window, "bank-feed-export"),
        *archive = Find(window, "bank-feed-archive-export"), *view = Find(window, "bank-feed-output"),
        *note = Find(window, "bank-feed-note");
    CHECK(path && format && load && save && archive && view && note);
    CHECK(GTK_IS_SCROLLED_WINDOW(gtk_window_get_child(window)));
    g_object_ref(load); g_object_ref(save); g_object_ref(archive); g_object_ref(view); g_object_ref(note);
    GtkWidget *exportPath = Tagged(archive, "text.export.path"), *exportSave = Tagged(archive, "text.export.save");
    CHECK(exportPath && exportSave); g_object_ref(exportSave);
    GError *error = NULL;
    char *directory = g_dir_make_tmp("umicom-bank-archive-XXXXXX", &error);
    CHECK(directory && error == NULL);
    char *filename = g_build_filename(directory, "source.csv", NULL);
    char *destination = g_build_filename(directory, "saved.umibank.json", NULL);
    char *source = BankFixture(BANK_TEST_ROW); CHECK(source != NULL);
    UmiBankFeedArchiveInfo info = {.sourceFormat = UMI_BANK_FEED_SOURCE_OPEN_BANKING_JSON};
    if (!strcmp(mode, "csv-source")) {
        free(source); source = malloc(strlen(BANK_CSV_TEXT) + 1U); CHECK(source != NULL); strcpy(source, BANK_CSV_TEXT);
        info.sourceFormat = UMI_BANK_FEED_SOURCE_STATEMENT_CSV;
    }
    CHECK(UmiSha256Buffer(source, strlen(source), info.sourceSha256) == UMI_STATUS_OK);
    bool raw = !strcmp(mode, "json-source") || !strcmp(mode, "csv-source");
    char *encoded = NULL; size_t encodedLength = 0U;
    if (!raw) {
        UmiBankFeed *feed = NULL;
        CHECK(UmiBankFeedCreate(source, strlen(source), &feed) == UMI_STATUS_OK);
        CHECK(UmiBankFeedArchiveEncode(feed, &info, &encoded, &encodedLength) == UMI_STATUS_OK);
        UmiBankFeedDestroy(feed);
    }
    const char *contents = raw ? source : encoded;
    if (!strcmp(mode, "invalid")) contents = "{\"format\":\"wrong\"}";
    CHECK(g_file_set_contents(filename, contents, -1, &error));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(format), raw ? (guint)info.sourceFormat - 1U : 3U);
    gtk_editable_set_text(GTK_EDITABLE(path), filename);
    gtk_editable_set_text(GTK_EDITABLE(exportPath), destination);
    ArchiveProbe probe = {window, path, format, !strcmp(mode, "close-notify"), 0U};
    gulong signal = 0U;
    if (!strcmp(mode, "edit-notify") || !strcmp(mode, "close-notify"))
        signal = g_signal_connect(archive, "notify::sensitive", G_CALLBACK(Published), &probe);
    if (!strcmp(mode, "prepare-change"))
        signal = g_signal_connect(note, "notify::label", G_CALLBACK(Prepared), &probe);
    g_signal_emit_by_name(load, "clicked");
    if (!strcmp(mode, "loading-change")) gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 0U);
    if (!strcmp(mode, "close-loading")) gtk_window_destroy(window);
    CHECK(Wait(window, archive));
    if (!strcmp(mode, "invalid") || !strcmp(mode, "loading-change") ||
        !strcmp(mode, "prepare-change") || !strcmp(mode, "edit-notify") ||
        !strcmp(mode, "close-notify") || !strcmp(mode, "close-loading")) {
        CHECK(!gtk_widget_get_sensitive(save) && !gtk_widget_get_sensitive(archive));
        if (!strcmp(mode, "edit-notify") || !strcmp(mode, "prepare-change") || !strcmp(mode, "close-notify"))
            CHECK(probe.calls > 0U);
        if (!strcmp(mode, "invalid")) CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "Import failed"));
    } else {
        CHECK(gtk_widget_get_sensitive(save) && gtk_widget_get_sensitive(archive));
        char *text = Text(view);
        CHECK(strstr(text, info.sourceSha256) && strstr(text, raw && info.sourceFormat == UMI_BANK_FEED_SOURCE_STATEMENT_CSV
            ? "Booked net movement: 10.25" : "Booked net movement: 12.50")); g_free(text);
        if (!strcmp(mode, "retained")) {
            gtk_window_destroy(window);
            g_signal_emit_by_name(exportSave, "clicked");
            g_signal_emit_by_name(load, "clicked");
            CHECK(!UmiTextExportGtk4Busy(archive) && !UmiBankFeedGtk4Busy(window));
            CHECK(!g_file_test(destination, G_FILE_TEST_EXISTS));
        } else {
            g_signal_emit_by_name(exportSave, "clicked");
            CHECK(Wait(window, archive));
            char *saved = NULL; gsize savedLength = 0U;
            CHECK(g_file_get_contents(destination, &saved, &savedLength, &error));
            UmiBankFeed *reopened = NULL; UmiBankFeedArchiveInfo retained;
            CHECK(UmiBankFeedArchiveDecode(saved, savedLength, &reopened, &retained) == UMI_STATUS_OK);
            CHECK(retained.sourceFormat == info.sourceFormat && !strcmp(retained.sourceSha256, info.sourceSha256));
            UmiBankFeedDestroy(reopened);
            if (!strcmp(mode, "exclusive-save")) {
                g_signal_emit_by_name(exportSave, "clicked"); CHECK(Wait(window, archive));
                GtkWidget *saveStatus = Tagged(archive, "text.export.status");
                CHECK(saveStatus && strstr(gtk_label_get_text(GTK_LABEL(saveStatus)), "Save failed"));
                char *same = NULL; gsize sameLength = 0U;
                CHECK(g_file_get_contents(destination, &same, &sameLength, &error));
                CHECK(sameLength == savedLength && memcmp(same, saved, savedLength) == 0); g_free(same);
            } else CHECK(!strcmp(mode, "roundtrip") || raw);
            g_free(saved);
            gtk_drop_down_set_selected(GTK_DROP_DOWN(format), 3U);
            gtk_editable_set_text(GTK_EDITABLE(path), destination);
            CHECK(!gtk_widget_get_sensitive(archive));
            g_signal_emit_by_name(load, "clicked"); CHECK(Wait(window, archive));
            text = Text(view); CHECK(strstr(text, info.sourceSha256)); g_free(text);
            CHECK(gtk_widget_get_sensitive(archive));
            CHECK(g_remove(destination) == 0);
        }
    }
    if (signal != 0U) g_signal_handler_disconnect(!strcmp(mode, "prepare-change") ? G_OBJECT(note) : G_OBJECT(archive), signal);
    gtk_window_destroy(window); CHECK(Wait(window, archive));
    CHECK(!gtk_widget_get_sensitive(archive));
    CHECK(g_remove(filename) == 0 && g_rmdir(directory) == 0);
    UmiBankFeedArchiveFree(encoded); free(source);
    g_free(filename); g_free(destination); g_free(directory); g_clear_error(&error);
    g_object_unref(exportSave); g_object_unref(load); g_object_unref(save); g_object_unref(archive);
    g_object_unref(view); g_object_unref(note); g_object_unref(window);
    return 0;
}
