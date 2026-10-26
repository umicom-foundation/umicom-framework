/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_comparison_workflow.c
 *
 * PURPOSE:
 *   Exercise retained-before/current-after imports through the shared bank window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
#include "csv_fixture.h"
#include "../support/main_context.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include <glib/gstdio.h>
#undef CHECK
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *widget = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(widget != NULL); return widget;
}
static void Click(GtkWidget *button) { g_signal_emit_by_name(button, "clicked"); }
static void Wait(GtkWindow *window) {
    gint64 deadline = g_get_monotonic_time() + 10000000;
    while (UmiBankFeedGtk4Busy(window) && g_get_monotonic_time() < deadline) {
        (void)UmiTestDispatchSlice(NULL, deadline, 32U);
        g_usleep(1000U);
    }
    CHECK(!UmiBankFeedGtk4Busy(window));
}
static GtkWindow *Review(void) {
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
        GtkWindow *window = g_list_model_get_item(windows, i);
        const char *title = gtk_window_get_title(window);
        if (title != NULL && !strcmp(title, "Compare imported bank observations")) return window;
        g_object_unref(window);
    }
    return NULL;
}
static void Import(GtkWindow *window, const char *path) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(window, "bank-feed-format")), 1U);
    gtk_editable_set_text(GTK_EDITABLE(Find(window, "bank-feed-path")), path);
    Click(Find(window, "bank-feed-load"));
    Wait(window);
}
typedef struct WorkflowProbe { GtkWindow *window; GtkWidget *button; bool close; unsigned calls; } WorkflowProbe;
static void Retained(GObject *object, GParamSpec *spec, gpointer data) {
    (void)object; (void)spec;
    WorkflowProbe *probe = data;
    if (probe->calls++ != 0U) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else Click(probe->button);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    GError *error = NULL;
    char *directory = g_dir_make_tmp("umicom-bank-comparison-XXXXXX", &error);
    CHECK(directory != NULL && error == NULL);
    char *first = g_build_filename(directory, "before.csv", NULL);
    char *second = g_build_filename(directory, "after.csv", NULL);
    char *invalid = g_build_filename(directory, "invalid.csv", NULL);
    CHECK(g_file_set_contents(first, BANK_CSV_HEADER BANK_CSV_CREDIT, -1, &error) && error == NULL);
    CHECK(g_file_set_contents(second, BANK_CSV_HEADER BANK_CSV_DEBIT, -1, &error) && error == NULL);
    CHECK(g_file_set_contents(invalid, "not a supported statement", -1, &error) && error == NULL);
    GtkWindow *window = UmiBankFeedGtk4Create(NULL); CHECK(window != NULL); g_object_ref(window);
    GtkWidget *retain = Find(window, "bank-feed-retain-before");
    GtkWidget *compare = Find(window, "bank-feed-compare-imports");
    GtkWidget *forget = Find(window, "bank-feed-forget-before");
    GtkWidget *note = Find(window, "bank-feed-comparison-note");
    g_object_ref(retain); g_object_ref(compare); g_object_ref(forget); g_object_ref(note);
    if (!strcmp(mode, "empty")) {
        Click(retain); Click(compare);
        CHECK(Review() == NULL);
    } else {
        Import(window, first);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(window, "bank-feed-note"))), "Imported locally") != NULL);
        if (!strcmp(mode, "retain-close") || !strcmp(mode, "retain-reentry")) {
            WorkflowProbe probe = {window, forget, !strcmp(mode, "retain-close"), 0U};
            g_signal_connect(note, "notify::label", G_CALLBACK(Retained), &probe);
            Click(retain); CHECK(probe.calls > 0U);
            g_signal_handlers_disconnect_by_data(note, &probe);
            if (!probe.close) CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "Retained before import") != NULL);
        } else {
            Click(retain);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "Retained before import: 1 rows") != NULL);
            if (!strcmp(mode, "clear-current")) {
                Click(Find(window, "bank-feed-clear")); Click(compare);
                CHECK(Review() == NULL);
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(note)), "retained") != NULL ||
                      strstr(gtk_label_get_text(GTK_LABEL(note)), "Retained") != NULL);
            }
            if (!strcmp(mode, "invalid-after")) {
                Import(window, invalid); Click(compare); CHECK(Review() == NULL);
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(window, "bank-feed-note"))), "Import failed") != NULL);
                Import(window, second);
            } else if (!strcmp(mode, "loading")) {
                gtk_editable_set_text(GTK_EDITABLE(Find(window, "bank-feed-path")), second);
                Click(Find(window, "bank-feed-load"));
                CHECK(UmiBankFeedGtk4Busy(window));
                Click(retain); Click(compare);
                CHECK(Review() == NULL);
                Wait(window);
            } else Import(window, second);
            if (!strcmp(mode, "replace-before")) Click(retain);
            if (!strcmp(mode, "forget")) {
                Click(forget); Click(compare);
                CHECK(Review() == NULL);
            } else if (!strcmp(mode, "retained-close")) {
                gtk_window_destroy(window);
                Click(retain); Click(compare); Click(forget);
                CHECK(Review() == NULL);
            } else {
                Click(compare);
                GtkWindow *review = Review(); CHECK(review != NULL);
                const char *summary = gtk_label_get_text(GTK_LABEL(Find(review, "bank-comparison-summary")));
                if (!strcmp(mode, "replace-before")) CHECK(strstr(summary, "Same: 1; Changed: 0") != NULL);
                else CHECK(strstr(summary, "Before-only: 1; After-only: 1") != NULL);
                /* A review owns both imports independently from this host's
                 * retained-side controls and later file invalidation. */
                char *saved = g_strdup(summary);
                Click(forget); Click(Find(window, "bank-feed-clear"));
                CHECK(!strcmp(saved, gtk_label_get_text(GTK_LABEL(Find(review, "bank-comparison-summary")))));
                g_free(saved);
                gtk_window_destroy(review); g_object_unref(review);
            }
        }
    }
    Wait(window);
    gtk_window_destroy(window);
    g_object_unref(note); g_object_unref(forget); g_object_unref(compare); g_object_unref(retain); g_object_unref(window);
    CHECK(g_remove(first) == 0 && g_remove(second) == 0 && g_remove(invalid) == 0 && g_rmdir(directory) == 0);
    g_free(first); g_free(second); g_free(invalid); g_free(directory);
    return 0;
}
