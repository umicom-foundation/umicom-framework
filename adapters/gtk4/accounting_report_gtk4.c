/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/accounting_report_gtk4.c
 *
 * PURPOSE:
 *   Keep accounting report capture, export ownership and window retirement in Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/accounting_report.h"
#include "umicom/ui/gtk4/text_export.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/open_tms/accounting_report.h"
#include "window_removal_private.h"

typedef struct AccountingReportPanel {
    GtkTextBuffer *buffer;
    GtkWidget *export;
    gboolean retired;
} AccountingReportPanel;
static void ReportRetire(GtkWidget *window, gpointer unused) {
    (void)unused;
    AccountingReportPanel *panel = g_object_get_data(G_OBJECT(window), "umicom-accounting-report");
    if (panel == NULL || panel->retired) return;
    g_object_ref(window);
    panel->retired = TRUE;
    UmiTextExportGtk4Retire(panel->export);
    gtk_widget_set_sensitive(window, FALSE);
    g_object_unref(window);
}
static void ReportDestroy(gpointer value) {
    AccountingReportPanel *panel = value;
    UmiTextExportGtk4Retire(panel->export);
    g_clear_object(&panel->export);
    g_clear_object(&panel->buffer);
    g_free(panel);
}
UmiStatus UmiGtk4AccountingReportCreate(GtkWindow *parent,
    const UmiOpenTmsAccountingBook *book, GtkWindow **outWindow) {
    if (outWindow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWindow = NULL;
    if (parent != NULL && (!GTK_IS_WINDOW(parent) || !UmiGtk4WindowRegistered(parent)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCsvDocument *report = NULL;
    UmiStatus status = umi_open_tms_accounting_book_report(book, &report);
    if (status != UMI_STATUS_OK) return status;

    /* Freeze values before creating observable UI. The report and its title
     * describe one capture, not a live claim about the changing source book. */
    uint64_t revision = book->revision;
    AccountingReportPanel *panel = g_new0(AccountingReportPanel, 1);
    panel->buffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(panel->buffer, UmiCsvDocumentData(report), -1);
    UmiCsvDocumentDestroy(report);
    panel->export = UmiTextExportGtk4CreateForBuffer(panel->buffer, "accounting-practice.csv");
    if (panel->export == NULL) { ReportDestroy(panel); return UMI_STATUS_OUT_OF_MEMORY; }
    g_object_ref_sink(panel->export);

    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref(window);
    g_object_set_data_full(G_OBJECT(window), "umicom-accounting-report", panel, ReportDestroy);
    gtk_widget_set_name(GTK_WIDGET(window), "treasury.accounting.report.window");
    gtk_window_set_title(window, "Accounting CSV — captured local practice");
    gtk_window_set_default_size(window, 960, 620);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
    }
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(box, 16); gtk_widget_set_margin_end(box, 16);
    gtk_widget_set_margin_top(box, 16); gtk_widget_set_margin_bottom(box, 16);
    gtk_window_set_child(window, box);
    char *caption = g_strdup_printf("Captured local revision %" G_GUINT64_FORMAT
        ". Later changes are not included.\nCurrencies remain separate; double-valued practice only. "
        "This report is not an archive, authenticated approval or ledger posting.", (guint64)revision);
    GtkWidget *note = gtk_label_new(caption); g_free(caption);
    gtk_widget_set_name(note, "treasury.accounting.report.note");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE); gtk_label_set_xalign(GTK_LABEL(note), 0.0F);
    gtk_box_append(GTK_BOX(box), note);
    GtkWidget *view = gtk_text_view_new_with_buffer(panel->buffer);
    gtk_widget_set_name(view, "treasury.accounting.report.csv");
    (void)umi_gtk4_automation_tag_widget(view, "treasury.accounting.report.csv");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_widget_set_name(panel->export, "treasury.accounting.report.export");
    (void)umi_gtk4_automation_tag_widget(panel->export, "treasury.accounting.report.export");
    gtk_box_append(GTK_BOX(box), panel->export);
    g_signal_connect(window, "destroy", G_CALLBACK(ReportRetire), NULL);
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), ReportRetire, NULL);
    if (parent != NULL)
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
    gboolean retired = panel->retired;
    if (!retired) *outWindow = window;
    g_object_unref(window);
    return retired ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
