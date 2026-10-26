/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/accounting_practice_gtk4.c
 *
 * PURPOSE:
 *   Compose validated accounting capture and complete-batch review with guarded GTK lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/accounting_practice.h"
#include "umicom/ui/gtk4/accounting_report.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/open_tms/accounting_review.h"
#include "umicom/open_tms/accounting_file.h"
#include "window_removal_private.h"
#include <errno.h>
#include <math.h>
#include <string.h>

enum { ACCOUNT, CURRENCY, REFERENCE, DEBIT, CREDIT, FIELD_COUNT };
typedef struct AccountingPanel {
    UmiOpenTmsAccountingBook book;
    UmiOpenTmsAccountingReview *review;
    GPtrArray *retained;
    GtkWidget *fields[FIELD_COUNT], *action, *ack, *apply, *note, *preview, *rows;
    uint64_t generation, acknowledged;
    gboolean busy, retired;
    /* Workers own snapshots; only the GTK owner may publish a reviewed replacement. */
    GtkWidget *filePath, *fileAck, *fileApply, *filePreview;
    UmiOpenTmsAccountingBook *expected;
    unsigned char *pendingBytes;
    size_t pendingSize;
    uint64_t fileGeneration, fileAcknowledged;
    gboolean fileBusy;
} AccountingPanel;
static AccountingPanel *Panel(gpointer window) {
    return g_object_get_data(G_OBJECT(window), "umicom-accounting-practice");
}
static GtkWidget *Keep(AccountingPanel *panel, GtkWidget *widget, const char *name) {
    g_ptr_array_add(panel->retained, g_object_ref_sink(widget));
    gtk_widget_set_name(widget, name);
    (void)umi_gtk4_automation_tag_widget(widget, name);
    return widget;
}
static void Invalidate(AccountingPanel *panel) {
    umi_open_tms_accounting_review_destroy(panel->review);
    panel->review = NULL; panel->acknowledged = 0U;
    g_clear_pointer(&panel->expected, g_free);
    g_clear_pointer(&panel->pendingBytes, g_free);
    panel->pendingSize = 0U; panel->fileGeneration = 0U; panel->fileAcknowledged = 0U;
    if (panel->generation != UINT64_MAX) ++panel->generation;
}
static void Destroy(gpointer value) {
    AccountingPanel *panel = value;
    umi_open_tms_accounting_review_destroy(panel->review);
    g_free(panel->expected); g_free(panel->pendingBytes);
    g_ptr_array_unref(panel->retained);
    g_free(panel);
}
static void Retire(GtkWidget *window, gpointer unused) {
    (void)unused;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return;
    g_object_ref(window);
    panel->retired = TRUE; Invalidate(panel);
    gtk_widget_set_sensitive(panel->apply, FALSE);
    gtk_widget_set_sensitive(window, FALSE);
    g_object_unref(window);
}
static gboolean Live(AccountingPanel *panel, uint64_t generation) {
    return !panel->retired && panel->generation == generation;
}
/* File and action reviews share one generation. Editing either workflow
 * invalidates both decisions before any native notification can reenter. */
static void RevokeFileDisplay(AccountingPanel *panel) {
    gtk_widget_set_sensitive(panel->fileApply, FALSE);
    if (panel->retired) return;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->fileAck), FALSE);
    if (panel->retired) return;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->filePreview)),
        "Open an archive to prepare a replacement review.", -1);
}
static void RevokeDisplay(AccountingPanel *panel) {
    RevokeFileDisplay(panel);
    if (panel->retired) return;
    gtk_widget_set_sensitive(panel->apply, FALSE);
    if (panel->retired) return;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->ack), FALSE);
    if (panel->retired) return;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->preview)),
        "Prepare a fresh review after any entry or action change.", -1);
}
static void Edited(GtkEditable *entry, gpointer window) {
    (void)entry;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return;
    g_object_ref(window);
    /* Revoke identity even during observable UI publication. The outer action
     * rechecks its generation; nested mutation buttons remain guarded. */
    Invalidate(panel);
    if (!panel->busy) {
        panel->busy = TRUE; RevokeDisplay(panel); panel->busy = FALSE;
    }
    g_object_unref(window);
}
static void ActionChanged(GObject *object, GParamSpec *spec, gpointer window) {
    (void)object; (void)spec; Edited(NULL, window);
}
static gboolean CopyText(GtkWidget *field, char *out, size_t capacity) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(field));
    size_t length = strlen(text);
    if (length == 0U || length >= capacity || !g_utf8_validate(text, -1, NULL)) return FALSE;
    memcpy(out, text, length + 1U); return TRUE;
}
static gboolean Amount(GtkWidget *field, double *out) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(field));
    size_t length = strlen(text);
    if (length == 0U || length > 63U) return FALSE;
    gboolean digit = FALSE, dot = FALSE;
    for (size_t i = 0U; i < length; ++i) {
        if (text[i] >= '0' && text[i] <= '9') digit = TRUE;
        else if (text[i] == '.' && !dot) dot = TRUE;
        else return FALSE;
    }
    if (!digit) return FALSE;
    errno = 0; char *end = NULL;
    double value = g_ascii_strtod(text, &end);
    if (errno == ERANGE || end == text || *end != '\0' || !isfinite(value)) return FALSE;
    *out = value; return TRUE;
}
static void AppendLine(GString *text, size_t index, const UmiOpenTmsAccountingLine *line) {
    char debit[G_ASCII_DTOSTR_BUF_SIZE], credit[G_ASCII_DTOSTR_BUF_SIZE];
    g_ascii_dtostr(debit, sizeof debit, line->debit);
    g_ascii_dtostr(credit, sizeof credit, line->credit);
    g_string_append_printf(text, "%zu. Account %s | %s | Debit %s | Credit %s | Reference %s\n",
        index + 1U, line->accountId, line->currency, debit, credit, line->referenceId);
}
static void RenderRows(AccountingPanel *panel) {
    GString *text = g_string_new("Local batch; no external posting\n");
    g_string_append_printf(text, "Revision %" G_GUINT64_FORMAT " | Lines %zu | Approved %s | Posted locally %s\n",
        (guint64)panel->book.revision, panel->book.count,
        panel->book.approved ? "yes" : "no", panel->book.posted ? "yes" : "no");
    for (size_t i = 0U; i < panel->book.count; ++i) AppendLine(text, i, &panel->book.lines[i]);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->rows)), text->str, -1);
    g_string_free(text, TRUE);
}
static void Capture(GtkButton *button, gpointer window) {
    (void)button;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE; Invalidate(panel);
    UmiOpenTmsAccountingLine line = {0};
    gboolean valid = CopyText(panel->fields[ACCOUNT], line.accountId, sizeof line.accountId) &&
        CopyText(panel->fields[CURRENCY], line.currency, sizeof line.currency) &&
        CopyText(panel->fields[REFERENCE], line.referenceId, sizeof line.referenceId) &&
        Amount(panel->fields[DEBIT], &line.debit) && Amount(panel->fields[CREDIT], &line.credit);
    UmiStatus status = valid ? umi_open_tms_accounting_book_add(&panel->book, &line) : UMI_STATUS_INVALID_ARGUMENT;
    RevokeDisplay(panel);
    if (!panel->retired) RenderRows(panel);
    if (!panel->retired)
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK ?
            "Line captured locally; any earlier approval is revoked." : umi_status_text(status));
    panel->busy = FALSE; g_object_unref(window);
}
static void Prepare(GtkButton *button, gpointer window) {
    (void)button;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE; Invalidate(panel);
    uint64_t generation = panel->generation;
    guint action = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->action));
    UmiStatus status = generation == UINT64_MAX ? UMI_STATUS_CAPACITY_EXCEEDED :
        action > 1U ? UMI_STATUS_INVALID_ARGUMENT :
        umi_open_tms_accounting_review_create(&panel->book,
            action == 0U ? UMI_OPEN_TMS_ACCOUNTING_APPROVE : UMI_OPEN_TMS_ACCOUNTING_POST, &panel->review);
    GString *text = NULL;
    if (status == UMI_STATUS_OK) {
        UmiOpenTmsAccountingReviewSnapshot snapshot;
        status = umi_open_tms_accounting_review_read(panel->review, &snapshot);
        if (status == UMI_STATUS_OK) {
            text = g_string_new("Review complete local batch\n");
            g_string_append_printf(text, "%s | Revision %" G_GUINT64_FORMAT " -> %" G_GUINT64_FORMAT
                "\nApproved %d -> %d | Posted locally %d -> %d\n",
                action == 0U ? "Approve locally" : "Set local posted flag",
                (guint64)snapshot.source_revision, (guint64)snapshot.next_revision,
                snapshot.approved_before, snapshot.approved_after, snapshot.posted_before, snapshot.posted_after);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot.line_count; ++i) {
                UmiOpenTmsAccountingLine row;
                status = umi_open_tms_accounting_review_line(panel->review, i, &row);
                if (status == UMI_STATUS_OK) AppendLine(text, i, &row);
            }
            g_string_append(text, "Each currency balances within 0.000001. Double-valued local preview only.\n"
                "No bank, ledger, file or authenticated approval will be changed.");
        }
    }
    /* Build the complete owned display before notifications can invalidate the
     * review. Never dereference the review after publishing observable text. */
    RevokeDisplay(panel);
    if (status == UMI_STATUS_OK && Live(panel, generation) && panel->review != NULL)
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->preview)), text->str, -1);
    if (Live(panel, generation))
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK ?
            "Read every line, then acknowledge this local action." : umi_status_text(status));
    if (!panel->retired && (!Live(panel, generation) || status != UMI_STATUS_OK || panel->review == NULL)) {
        Invalidate(panel); RevokeDisplay(panel);
    }
    if (text != NULL) g_string_free(text, TRUE);
    panel->busy = FALSE; g_object_unref(window);
}
static void Acknowledge(GtkCheckButton *check, gpointer window) {
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE;
    gboolean active = gtk_check_button_get_active(check) && panel->review != NULL;
    uint64_t generation = panel->generation;
    panel->acknowledged = active ? generation : 0U;
    gtk_widget_set_sensitive(panel->apply, active);
    if (!Live(panel, generation) || panel->review == NULL || !gtk_check_button_get_active(check)) {
        panel->acknowledged = 0U; gtk_widget_set_sensitive(panel->apply, FALSE);
    }
    panel->busy = FALSE; g_object_unref(window);
}
static void Apply(GtkButton *button, gpointer window) {
    (void)button;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy || panel->review == NULL ||
        panel->acknowledged != panel->generation || !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->ack))) return;
    g_object_ref(window); panel->busy = TRUE;
    UmiStatus status = umi_open_tms_accounting_review_apply(&panel->book, panel->review);
    Invalidate(panel); RevokeDisplay(panel);
    if (!panel->retired) RenderRows(panel);
    if (!panel->retired)
        gtk_label_set_text(GTK_LABEL(panel->note), status == UMI_STATUS_OK ?
            "Local batch flags updated. No external or ledger posting occurred." : umi_status_text(status));
    panel->busy = FALSE; g_object_unref(window);
}
#include "accounting_practice_files.inc"

/* Report windows own immutable captures. Keeping this composition in Framework
 * lets TMS and other hosts use the same validation and export lifetime rules. */
static void ReportOpen(GtkButton *button, gpointer window) {
    (void)button;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired || panel->busy) return;
    g_object_ref(window); panel->busy = TRUE;
    GtkWindow *report = NULL;
    UmiStatus status = UmiGtk4AccountingReportCreate(GTK_WINDOW(window), &panel->book, &report);
    if (status == UMI_STATUS_OK) {
        g_object_ref(report);
        if (!panel->retired) gtk_window_present(report);
        else gtk_window_destroy(report);
        g_object_unref(report);
    } else if (!panel->retired) gtk_label_set_text(GTK_LABEL(panel->note), umi_status_text(status));
    panel->busy = FALSE; g_object_unref(window);
}


static GtkWidget *Button(AccountingPanel *panel, GtkWidget *box, GtkWindow *window,
    const char *label, const char *name, GCallback callback) {
    GtkWidget *button = Keep(panel, gtk_button_new_with_label(label), name);
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect_object(button, "clicked", callback, window, 0);
    return button;
}
static GtkWidget *TextArea(AccountingPanel *panel, GtkWidget *box, const char *name) {
    GtkWidget *view = Keep(panel, gtk_text_view_new(), name), *scroll = gtk_scrolled_window_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_widget_set_size_request(scroll, -1, 180);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_box_append(GTK_BOX(box), scroll); return view;
}
GtkWindow *UmiGtk4AccountingPracticeCreate(GtkWindow *parent) {
    if (parent != NULL && (!GTK_IS_WINDOW(parent) || !UmiGtk4WindowRegistered(parent))) return NULL;
    AccountingPanel *panel = g_new0(AccountingPanel, 1);
    panel->retained = g_ptr_array_new_with_free_func(g_object_unref);
    panel->generation = 1U; panel->busy = TRUE;
    umi_open_tms_accounting_book_init(&panel->book);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref(window);
    g_object_set_data_full(G_OBJECT(window), "umicom-accounting-practice", panel, Destroy);
    gtk_widget_set_name(GTK_WIDGET(window), "treasury.accounting.window");
    gtk_window_set_title(window, "Accounting batch — local practice");
    gtk_window_set_default_size(window, 920, 760);
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent); gtk_window_set_destroy_with_parent(window, TRUE);
    }
    GtkWidget *scroll = gtk_scrolled_window_new(), *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_start(root, 16); gtk_widget_set_margin_end(root, 16);
    gtk_widget_set_margin_top(root, 16); gtk_widget_set_margin_bottom(root, 16);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), root); gtk_window_set_child(window, scroll);
    /* Retain the original in-memory-only introduction for review. Explicit
     * save/import now preserves a capture without implying a durable ledger. */
#if 0
    GtkWidget *intro = gtk_label_new("Independent in-memory batch. Closing discards all lines.\n"
        "Double-valued practice only; no authenticated approval, bank connection or ledger posting.");
#endif
    GtkWidget *intro = gtk_label_new("Independent local batch. Save a capture before closing to retain it.\n"
        "Double-valued practice only; imported flags are not authenticated approval or ledger posting.");
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE); gtk_box_append(GTK_BOX(root), intro);
    static const char *labels[] = {"Account ID", "Currency (three uppercase letters)", "Reference", "Debit", "Credit"};
    static const char *names[] = {"account", "currency", "reference", "debit", "credit"};
    static const char *initial[] = {"practice-cash", "GBP", "local-example", "100", "0"};
    for (size_t i = 0U; i < FIELD_COUNT; ++i) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        GtkWidget *caption = gtk_label_new(labels[i]);
        gtk_widget_set_size_request(caption, 220, -1); gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
        gtk_box_append(GTK_BOX(row), caption);
        char *name = g_strdup_printf("treasury.accounting.%s", names[i]);
        panel->fields[i] = Keep(panel, gtk_entry_new(), name); g_free(name);
        gtk_entry_set_max_length(GTK_ENTRY(panel->fields[i]), 63);
        gtk_editable_set_text(GTK_EDITABLE(panel->fields[i]), initial[i]);
        gtk_widget_set_hexpand(panel->fields[i], TRUE);
        gtk_box_append(GTK_BOX(row), panel->fields[i]); gtk_box_append(GTK_BOX(root), row);
        g_signal_connect_object(panel->fields[i], "changed", G_CALLBACK(Edited), window, 0);
    }
    Button(panel, root, window, "Add line to local batch", "treasury.accounting.capture", G_CALLBACK(Capture));
    panel->rows = TextArea(panel, root, "treasury.accounting.rows");
    Button(panel, root, window, "Open captured CSV report", "treasury.accounting.report", G_CALLBACK(ReportOpen));
    const char *actions[] = {"Approve local batch", "Set local posted flag", NULL};
    panel->action = Keep(panel, gtk_drop_down_new_from_strings(actions), "treasury.accounting.action");
    gtk_box_append(GTK_BOX(root), panel->action);
    g_signal_connect_object(panel->action, "notify::selected", G_CALLBACK(ActionChanged), window, 0);
    Button(panel, root, window, "Prepare complete batch review", "treasury.accounting.prepare", G_CALLBACK(Prepare));
    panel->preview = TextArea(panel, root, "treasury.accounting.preview");
    panel->ack = Keep(panel, gtk_check_button_new_with_label("I reviewed every line and this local action"), "treasury.accounting.ack");
    gtk_box_append(GTK_BOX(root), panel->ack);
    g_signal_connect_object(panel->ack, "toggled", G_CALLBACK(Acknowledge), window, 0);
    panel->apply = Button(panel, root, window, "Apply reviewed local action", "treasury.accounting.apply", G_CALLBACK(Apply));
    panel->note = Keep(panel, gtk_label_new("Add both sides of a balanced local batch to begin."), "treasury.accounting.note");
    gtk_label_set_wrap(GTK_LABEL(panel->note), TRUE); gtk_box_append(GTK_BOX(root), panel->note);
    GtkWidget *pathLabel = gtk_label_new("Archive file (absolute path in a directory you control)");
    gtk_label_set_xalign(GTK_LABEL(pathLabel), 0.0F); gtk_box_append(GTK_BOX(root), pathLabel);
    panel->filePath = Keep(panel, gtk_entry_new(), "treasury.accounting.path");
    gtk_entry_set_max_length(GTK_ENTRY(panel->filePath), (int)UMI_PATH_CAPACITY - 1);
    gtk_box_append(GTK_BOX(root), panel->filePath);
    g_signal_connect_object(panel->filePath, "changed", G_CALLBACK(Edited), window, 0);
    GtkWidget *files = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(root), files);
    Button(panel, files, window, "Save captured batch to new file", "treasury.accounting.save", G_CALLBACK(FileStart));
    GtkWidget *open = Button(panel, files, window, "Open archive for review", "treasury.accounting.open", G_CALLBACK(FileStart));
    g_object_set_data(G_OBJECT(open), "accounting-file-load", GINT_TO_POINTER(1));
    panel->filePreview = TextArea(panel, root, "treasury.accounting.file-preview");
    panel->fileAck = Keep(panel, gtk_check_button_new_with_label("I reviewed the current and loaded batches"),
        "treasury.accounting.file-ack");
    gtk_box_append(GTK_BOX(root), panel->fileAck);
    g_signal_connect_object(panel->fileAck, "toggled", G_CALLBACK(FileAcknowledge), window, 0);
    panel->fileApply = Button(panel, root, window, "Replace with reviewed local batch",
        "treasury.accounting.file-apply", G_CALLBACK(FileApply));
    RevokeDisplay(panel); RenderRows(panel); panel->busy = FALSE;
    g_signal_connect(window, "destroy", G_CALLBACK(Retire), NULL);
    UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), Retire, NULL);
    if (parent != NULL)
        UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
    gboolean retired = panel->retired;
    g_object_unref(window); return retired ? NULL : window;
}
UmiStatus UmiGtk4AccountingPracticeRead(GtkWindow *window, UmiOpenTmsAccountingBook *out_book) {
    if (window == NULL || out_book == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    AccountingPanel *panel = Panel(window);
    if (panel == NULL || panel->retired) return UMI_STATUS_INVALID_STATE;
    memcpy(out_book, &panel->book, sizeof *out_book); return UMI_STATUS_OK;
}
static void Open(GtkButton *button, gpointer unused) {
    (void)unused;
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (!GTK_IS_WINDOW(root) || !UmiGtk4WindowRegistered(GTK_WINDOW(root))) return;
    GtkWindow *window = UmiGtk4AccountingPracticeCreate(GTK_WINDOW(root));
    if (window != NULL) gtk_window_present(window);
}
GtkWidget *UmiGtk4AccountingPracticeLauncherCreate(void) {
    GtkWidget *button = gtk_button_new_with_label("Accounting batch practice");
    gtk_widget_set_name(button, "treasury.accounting.launch");
    (void)umi_gtk4_automation_tag_widget(button, "treasury.accounting.launch");
    g_signal_connect(button, "clicked", G_CALLBACK(Open), NULL); return button;
}
