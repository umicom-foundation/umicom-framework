/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_feed/test_comparison_native.c
 *
 * PURPOSE:
 *   Qualify immutable native comparison, explicit filtering and retired controls.
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
#include "umicom/bank_feed/csv_import.h"
#include "umicom/ui/gtk4/bank_comparison.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/text_export.h"
#undef CHECK
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); exit(1); } } while (0)
static GtkWidget *Find(GtkWindow *window, const char *name) {
    GtkWidget *result = umi_gtk4_automation_find_named_widget(GTK_WIDGET(window), name);
    CHECK(result != NULL);
    return result;
}
static char *Text(GtkWidget *view) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
}
typedef struct Probe {
    GtkWindow *window;
    GtkWidget *selection;
    gboolean close;
    unsigned calls;
} Probe;
static void Notified(GObject *object, GParamSpec *spec, gpointer data) {
    (void)object; (void)spec;
    Probe *probe = data;
    if (probe->calls++ != 0U) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else gtk_drop_down_set_selected(GTK_DROP_DOWN(probe->selection), 0U);
}
static void TextChanged(GtkTextBuffer *buffer, gpointer data) {
    Notified(G_OBJECT(buffer), NULL, data);
}
int main(int argc, char **argv) {
    CHECK(argc == 2);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    /* The dropdown model reserves row zero for the explicit empty choice. */
    const char *a = !strcmp(mode, "empty") ? BANK_CSV_HEADER : BANK_CSV_HEADER BANK_CSV_CREDIT BANK_CSV_DEBIT;
    const char *b = !strcmp(mode, "empty") ? BANK_CSV_HEADER : BANK_CSV_HEADER BANK_CSV_CREDIT BANK_CSV_PENDING;
    UmiBankFeed *before = NULL, *after = NULL;
    CHECK(UmiBankFeedCsvCreate(a, strlen(a), &before, NULL) == UMI_STATUS_OK);
    CHECK(UmiBankFeedCsvCreate(b, strlen(b), &after, NULL) == UMI_STATUS_OK);
    GtkWindow *parent = NULL;
    if (!strcmp(mode, "parent") || !strcmp(mode, "removed-parent")) {
        parent = GTK_WINDOW(gtk_window_new()); g_object_ref(parent);
        if (!strcmp(mode, "removed-parent")) gtk_window_destroy(parent);
    }
    if (!strcmp(mode, "invalid-input") || !strcmp(mode, "removed-parent")) {
        CHECK(UmiBankFeedComparisonGtk4Create(!strcmp(mode, "invalid-input") ? NULL : before,
                                               after, parent) == NULL);
        if (!strcmp(mode, "invalid-input"))
            CHECK(UmiBankFeedComparisonGtk4Create(before, NULL, NULL) == NULL);
        UmiBankFeedDestroy(before); UmiBankFeedDestroy(after);
        if (parent != NULL) g_object_unref(parent);
        return 0;
    }
    GtkWindow *window = UmiBankFeedComparisonGtk4Create(before, after, parent);
    CHECK(window != NULL); g_object_ref(window);
    UmiBankFeedDestroy(before); UmiBankFeedDestroy(after);
    GtkWidget *filter = Find(window, "bank-comparison-filter");
    GtkWidget *rows = Find(window, "bank-comparison-rows");
    GtkWidget *view = Find(window, "bank-comparison-detail");
    GtkWidget *summary = Find(window, "bank-comparison-summary");
    GtkWidget *save = Find(window, "bank-comparison-export");
    g_object_ref(filter); g_object_ref(rows); g_object_ref(view); g_object_ref(save);
    CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(view)));
    CHECK(gtk_widget_get_sensitive(save));
    if (!strcmp(mode, "normal")) {
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(summary)), "Same: 1") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(summary)), "Before-only: 1; After-only: 1") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(summary)), "Absence is not deletion") != NULL);
        char *text = Text(view);
        CHECK(strstr(text, "BEFORE IMPORT") && strstr(text, "AFTER IMPORT") && strstr(text, "credit-one"));
        g_free(text);
    } else if (!strcmp(mode, "filter")) {
/* Count the prompt separately from the three financial observations. The previous implementation is retained for review. */
#if 0
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 3U);
#endif
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 4U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), 1U);
/* The filtered model contains a prompt and two unmatched observations. The previous implementation is retained for review. */
#if 0
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 2U);
#endif
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 3U);
        char *text = Text(view); CHECK(strstr(text, "before-only") && strstr(text, "debit-one")); g_free(text);
/* Use the after-only observation after the prompt and before-only observation. The previous implementation is retained for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 1U);
        text = Text(view);
#endif
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 2U);
        text = Text(view); CHECK(strstr(text, "after-only") && strstr(text, "pending-one")); g_free(text);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), 0U);
/* Restoring all observations retains the explicit prompt. The previous implementation is retained for review. */
#if 0
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 3U);
#endif
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 4U);
    } else if (!strcmp(mode, "empty")) {
/* An empty comparison still offers its neutral prompt. The previous implementation is retained for review. */
#if 0
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 0U);
#endif
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 1U);
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(rows)) == 0U);
        char *text = Text(view); CHECK(strstr(text, "Choose an observation") != NULL); g_free(text);
    } else if (!strcmp(mode, "invalid-selection")) {
/* Exercise the supported clear-selection action instead of a position GTK can automatically replace. The previous implementation is retained for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), GTK_INVALID_LIST_POSITION);
#endif
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 0U);
        char *text = Text(view); CHECK(strstr(text, "Choose an observation") != NULL); g_free(text);
    } else if (!strcmp(mode, "filter-close") || !strcmp(mode, "filter-reentry")) {
        Probe probe = {window, filter, !strcmp(mode, "filter-close"), 0U};
        g_signal_connect(rows, "notify::model", G_CALLBACK(Notified), &probe);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), 1U);
        CHECK(probe.calls > 0U);
        if (!probe.close) {
            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(filter)) == 0U);
/* GObject restarts a reentered notify signal. Assert that the settled model and details match the latest filter, rather than observing a temporary discarded model. The previous implementation is retained for review. */
#if 0
            CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(rows)) == NULL);
            char *text = Text(view); CHECK(strstr(text, "Choose an observation") != NULL); g_free(text);
#endif
            CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(rows))) == 4U);
            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(rows)) == 1U);
            char *text = Text(view);
            CHECK(strstr(text, "credit-one") && strstr(text, "BEFORE IMPORT") && strstr(text, "AFTER IMPORT"));
            g_free(text);
        }
        g_signal_handlers_disconnect_by_data(rows, &probe);
    } else if (!strcmp(mode, "detail-close") || !strcmp(mode, "detail-reentry")) {
        Probe probe = {window, rows, !strcmp(mode, "detail-close"), 0U};
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
        g_signal_connect(buffer, "changed", G_CALLBACK(TextChanged), &probe);
/* Move away from the initially selected observation so the detail callback runs. The previous implementation is retained for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 1U);
#endif
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 2U);
        CHECK(probe.calls > 0U);
        if (!probe.close) {
/* A reentered selection notification settles on the prompt. Ensure it cannot leave stale financial details under that choice. The previous implementation is retained for review. */
#if 0
            char *text = Text(view); CHECK(strstr(text, "Selection changed during review") != NULL); g_free(text);
#endif
            CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(rows)) == 0U);
            char *text = Text(view);
            CHECK(strstr(text, "Choose an observation") != NULL);
            CHECK(strstr(text, "BEFORE IMPORT") == NULL && strstr(text, "AFTER IMPORT") == NULL);
            g_free(text);
        }
        g_signal_handlers_disconnect_by_data(buffer, &probe);
    } else if (!strcmp(mode, "parent") || !strcmp(mode, "retained")) {
        if (parent != NULL) gtk_window_destroy(parent); else gtk_window_destroy(window);
        char *kept = Text(view);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(filter), 1U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(rows), 1U);
        char *now = Text(view); CHECK(!strcmp(kept, now)); g_free(kept); g_free(now);
        GtkWidget *button = umi_gtk4_automation_find_tagged_widget(save, "text.export.save");
        CHECK(button != NULL); g_signal_emit_by_name(button, "clicked");
        CHECK(!UmiTextExportGtk4Busy(save));
    } else CHECK(false);
    gtk_window_destroy(window);
    if (parent != NULL) { gtk_window_destroy(parent); g_object_unref(parent); }
    g_object_unref(save); g_object_unref(view); g_object_unref(rows); g_object_unref(filter);
    g_object_unref(window);
    return 0;
}
