/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_comparison_gtk4.c
 *
 * PURPOSE:
 *   Present immutable imported-bank differences without changing financial data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/bank_comparison.h"
#include "umicom/ui/gtk4/text_export.h"
#include "window_removal_private.h"
#include <string.h>
#define COMPARISON_UI "umicom-bank-comparison-window"
typedef struct ComparisonUi {
    UmiBankFeedComparison *comparison;
    GtkWidget *filter, *rows, *summary, *view, *save;
    GtkTextBuffer *csv;
    size_t displayed[2U * UMI_BANK_FEED_MAX_ROWS], count;
    gboolean closed, publishing;
} ComparisonUi;

static ComparisonUi *ComparisonOwner(GtkWindow *window) {
    ComparisonUi *ui = g_object_get_data(G_OBJECT(window), COMPARISON_UI);
    return ui != NULL && !ui->closed && UmiGtk4WindowRegistered(window) ? ui : NULL;
}
static void ComparisonFree(gpointer data) {
    ComparisonUi *ui = data;
    UmiBankFeedComparisonDestroy(ui->comparison);
    g_clear_object(&ui->csv);
    g_clear_object(&ui->save);
    g_free(ui);
}
static void ComparisonClosed(GtkWidget *widget, gpointer unused) {
    (void)unused;
    ComparisonUi *ui = g_object_get_data(G_OBJECT(widget), COMPARISON_UI);
    if (ui == NULL || ui->closed) return;
    ui->closed = TRUE;
    UmiBankFeedComparisonDestroy(ui->comparison);
    ui->comparison = NULL;
    UmiTextExportGtk4Retire(ui->save);
}
static void ComparisonAppendSide(GString *text, const char *side, size_t index,
                       const UmiBankFeedRow *row) {
    g_string_append_printf(text, "\n%s\n", side);
    if (index == SIZE_MAX) {
        g_string_append(text, "Not observed on this imported side. This does not prove deletion.\n");
        return;
    }
    g_string_append_printf(text,
        "Original row (zero-based): %zu\nAccount: %s\nTransaction ID: %s\n"
        "Booking time: %s\nAmount: %s %s (%s)\nStatus: %s; %s\nReference: %s\nInformation: %s\n",
        index, row->account, row->transaction[0] ? row->transaction : "(not supplied)",
        row->bookingTime, row->amountText, row->currency, row->credit ? "Credit" : "Debit",
        row->booked ? "Booked" : "Pending", row->mutableRecord ? "Mutable" : "Immutable",
        row->reference, row->information);
}
static void ComparisonAppendChanges(GString *text, uint32_t changes) {
    g_string_append(text, "Changed fields:");
#define FIELD(flag, name) do { if ((changes & flag) != 0U) g_string_append(text, " " name ";"); } while (0)
    FIELD(UMI_BANK_FEED_CHANGE_AMOUNT, "exact amount");
    FIELD(UMI_BANK_FEED_CHANGE_AMOUNT_TEXT, "amount spelling");
    FIELD(UMI_BANK_FEED_CHANGE_CURRENCY, "currency");
    FIELD(UMI_BANK_FEED_CHANGE_DIRECTION, "credit/debit");
    FIELD(UMI_BANK_FEED_CHANGE_STATUS, "booked/pending");
    FIELD(UMI_BANK_FEED_CHANGE_MUTABILITY, "mutability");
    FIELD(UMI_BANK_FEED_CHANGE_BOOKING_TIME, "booking time");
    FIELD(UMI_BANK_FEED_CHANGE_REFERENCE, "reference");
    FIELD(UMI_BANK_FEED_CHANGE_INFORMATION, "information");
#undef FIELD
    if (changes == 0U) g_string_append(text, " none (one-sided rows are not field changes)");
    g_string_append_c(text, '\n');
}
static void ComparisonSelection(GObject *object, GParamSpec *spec, gpointer data) {
    (void)object; (void)spec;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ComparisonUi *ui = ComparisonOwner(window);
    if (ui != NULL && !ui->publishing) {
        GString *text = g_string_new("Choose an observation to inspect both imported sides.\n");
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->rows));
/* Row zero is an explicit empty choice because GtkDropDown may restore an automatic selection when given an invalid position. The previous implementation is retained for review. */
#if 0
        if ((size_t)selected < ui->count) {
#endif
        if (selected != 0U && (size_t)selected <= ui->count) {
            UmiBankFeedComparisonRow row;
/* Translate the visible row after the prompt to the immutable comparison index. The previous implementation is retained for review. */
#if 0
            if (UmiBankFeedComparisonAt(ui->comparison, ui->displayed[selected], &row) == UMI_STATUS_OK) {
#endif
            if (UmiBankFeedComparisonAt(ui->comparison, ui->displayed[selected - 1U], &row) == UMI_STATUS_OK) {
                g_string_assign(text, UmiBankFeedComparisonKindName(row.kind));
                g_string_append_c(text, '\n');
                ComparisonAppendChanges(text, row.changes);
                ComparisonAppendSide(text, "BEFORE IMPORT", row.beforeIndex, &row.before);
                ComparisonAppendSide(text, "AFTER IMPORT", row.afterIndex, &row.after);
            }
        }
        /* Format before observable GTK setters. Closing in a buffer callback
         * may release the model; no borrowed row survives publication. */
        ui->publishing = TRUE;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->view)), text->str, -1);
        ui = ComparisonOwner(window);
        if (ui != NULL && gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->rows)) != selected) {
            /* A host may change selection from a text notification. Do not
             * leave the previous row labelled as the newly selected row. */
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->view)),
                "Selection changed during review. Choose an observation again.", -1);
            ui = ComparisonOwner(window);
        }
        if (ui != NULL) ui->publishing = FALSE;
        g_string_free(text, TRUE);
    }
    g_object_unref(window);
}
static void ComparisonFilter(GObject *object, GParamSpec *spec, gpointer data) {
    (void)object; (void)spec;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ComparisonUi *ui = ComparisonOwner(window);
    if (ui == NULL || ui->publishing) { g_object_unref(window); return; }
    const guint filter = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->filter));
    UmiBankFeedComparisonSummary summary;
    if (UmiBankFeedComparisonRead(ui->comparison, &summary) != UMI_STATUS_OK) {
        g_object_unref(window); return;
    }
    GtkStringList *choices = gtk_string_list_new(NULL);
    /* A real prompt row gives users and accessibility clients a stable way
     * to clear details without depending on automatic-selection policy. */
    gtk_string_list_append(choices, "Choose an observation");
    ui->count = 0U;
    for (size_t i = 0U; i < summary.rows; ++i) {
        UmiBankFeedComparisonRow row;
        if (UmiBankFeedComparisonAt(ui->comparison, i, &row) != UMI_STATUS_OK) continue;
        if (filter == 1U && row.kind == UMI_BANK_FEED_SAME) continue;
        const UmiBankFeedRow *identity = row.beforeIndex != SIZE_MAX ? &row.before : &row.after;
        char *label = g_strdup_printf("%zu | %s | %s | %s", i,
            UmiBankFeedComparisonKindName(row.kind), identity->account,
            identity->transaction[0] ? identity->transaction : "(no transaction ID)");
        gtk_string_list_append(choices, label);
        g_free(label);
        ui->displayed[ui->count++] = i;
    }
    ui->publishing = TRUE;
    gtk_drop_down_set_model(GTK_DROP_DOWN(ui->rows), G_LIST_MODEL(choices));
    g_object_unref(choices);
    ui = ComparisonOwner(window);
    if (ui != NULL) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->rows),
/* Select the first observation when present and the prompt for an empty comparison. The previous implementation is retained for review. */
#if 0
            ui->count == 0U ? GTK_INVALID_LIST_POSITION : 0U);
#endif
            ui->count == 0U ? 0U : 1U);
        ui = ComparisonOwner(window);
        if (ui != NULL) {
            if (gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->filter)) != filter) {
                /* Keep a reentrant filter edit visible, but discard its older
                 * display mapping instead of pretending it matches that edit. */
                ui->count = 0U;
                gtk_drop_down_set_model(GTK_DROP_DOWN(ui->rows), NULL);
                ui = ComparisonOwner(window);
            }
            if (ui != NULL) {
                ui->publishing = FALSE;
                ComparisonSelection(NULL, NULL, window);
            }
        }
    }
    g_object_unref(window);
}
static GtkWidget *ComparisonNamed(GtkWidget *widget, const char *name) {
    gtk_widget_set_name(widget, name);
    return widget;
}
GtkWindow *UmiBankFeedComparisonGtk4Create(const UmiBankFeed *before,
                                          const UmiBankFeed *after,
                                          GtkWindow *parent) {
    if (parent != NULL && !UmiGtk4WindowRegistered(parent)) return NULL;
    UmiBankFeedComparison *comparison = NULL;
    if (UmiBankFeedComparisonCreate(before, after, &comparison) != UMI_STATUS_OK) return NULL;
    UmiBankFeedComparisonSummary summary;
    if (UmiBankFeedComparisonRead(comparison, &summary) != UMI_STATUS_OK) {
        UmiBankFeedComparisonDestroy(comparison); return NULL;
    }
    UmiCsvDocument *csv = NULL;
    UmiStatus exportStatus = UmiBankFeedComparisonExportCsv(comparison, &csv);
    if (exportStatus == UMI_STATUS_OK &&
        g_utf8_strlen(UmiCsvDocumentData(csv), (gssize)UmiCsvDocumentBytes(csv)) > UMI_TEXT_EXPORT_MAX_CHARACTERS)
        exportStatus = UMI_STATUS_CAPACITY_EXCEEDED;
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    ComparisonUi *ui = g_new0(ComparisonUi, 1);
    ui->comparison = comparison;
    g_object_set_data_full(G_OBJECT(window), COMPARISON_UI, ui, ComparisonFree);
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), ComparisonClosed, NULL);
    gtk_window_set_title(window, "Compare imported bank observations");
    gtk_window_set_default_size(window, 1000, 720);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
    }
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(root, 16); gtk_widget_set_margin_end(root, 16);
    gtk_widget_set_margin_top(root, 16); gtk_widget_set_margin_bottom(root, 16);
    GtkWidget *form = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(form), root);
    gtk_window_set_child(window, form);
    char *summaryText = g_strdup_printf(
        "Before: %zu rows; After: %zu rows\nSame: %zu; Changed: %zu; Before-only: %zu; After-only: %zu\n"
        "Unidentified before: %zu; Unidentified after: %zu\n"
        "Before previous/next page: %s/%s; After previous/next page: %s/%s\n"
        "All imported accounts are included. Completeness, authenticity and chronology are unverified.\n"
        "Absence is not deletion. Nothing is merged or posted.",
        summary.before.rows, summary.after.rows, summary.same, summary.changed, summary.beforeOnly, summary.afterOnly,
        summary.beforeUnidentified, summary.afterUnidentified,
        summary.before.hasPreviousPage ? "yes" : "no", summary.before.hasNextPage ? "yes" : "no",
        summary.after.hasPreviousPage ? "yes" : "no", summary.after.hasNextPage ? "yes" : "no");
    ui->summary = ComparisonNamed(gtk_label_new(summaryText), "bank-comparison-summary");
    g_free(summaryText);
    gtk_label_set_wrap(GTK_LABEL(ui->summary), TRUE);
    gtk_label_set_selectable(GTK_LABEL(ui->summary), TRUE);
    gtk_label_set_xalign(GTK_LABEL(ui->summary), 0.0f);
    gtk_box_append(GTK_BOX(root), ui->summary);
    const char *filters[] = {"All observations", "Hide identical pairs", NULL};
    ui->filter = ComparisonNamed(gtk_drop_down_new_from_strings(filters), "bank-comparison-filter");
    gtk_box_append(GTK_BOX(root), ui->filter);
    ui->rows = ComparisonNamed(gtk_drop_down_new(NULL, NULL), "bank-comparison-rows");
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(ui->rows), TRUE);
    gtk_box_append(GTK_BOX(root), ui->rows);
    ui->view = ComparisonNamed(gtk_text_view_new(), "bank-comparison-detail");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ui->view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(ui->view), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), ui->view);
    gtk_widget_set_size_request(scroll, -1, 240);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(root), scroll);
    ui->csv = gtk_text_buffer_new(NULL);
    if (exportStatus == UMI_STATUS_OK)
        gtk_text_buffer_set_text(ui->csv, UmiCsvDocumentData(csv), -1);
    UmiCsvDocumentDestroy(csv);
    GtkWidget *notice = gtk_label_new(exportStatus == UMI_STATUS_OK
        ? "CSV exports both sides of every comparison, including identical pairs hidden by the filter."
        : "CSV export is unavailable: allocation or export size limit. The observation review remains available.");
    gtk_label_set_wrap(GTK_LABEL(notice), TRUE);
    gtk_widget_set_name(notice, "bank-comparison-export-note");
    gtk_box_append(GTK_BOX(root), notice);
    ui->save = UmiTextExportGtk4CreateForBuffer(ui->csv, "bank-import-comparison.csv");
    gtk_widget_set_name(ui->save, "bank-comparison-export");
    gtk_widget_set_sensitive(ui->save, exportStatus == UMI_STATUS_OK);
    gtk_box_append(GTK_BOX(root), ui->save);
    g_object_ref(ui->save);
    g_signal_connect_object(ui->filter, "notify::selected", G_CALLBACK(ComparisonFilter), window, 0);
    g_signal_connect_object(ui->rows, "notify::selected", G_CALLBACK(ComparisonSelection), window, 0);
    ComparisonFilter(NULL, NULL, window);
    return window;
}
