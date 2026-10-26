/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_statement_gtk4.c
 * PURPOSE: Keep editable statement assumptions separate from an owned imported snapshot.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/bank_statement.h"
#include "umicom/bank_feed/statement_timeline.h"
#include "umicom/bank_feed/statement_archive.h"
#include <stdio.h>
#include "umicom/ui/gtk4/text_export.h"
#include "window_removal_private.h"
#include <string.h>
#define STATEMENT_UI "umicom-bank-statement-window"
typedef struct StatementUi {
  UmiBankFeed *feed;
  char *account, *currency;
  GtkWidget *first, *last, *opening, *closing, *result, *save;
  GtkTextBuffer *csv;
  GtkTextBuffer *dailyText, *dailyCsv; /* Owned independently of view lifetime. */
  GtkWidget *dailySave;
  UmiBankFeedArchiveInfo source;
  GtkTextBuffer *reviewArchive;
  GtkWidget *reviewSave; /* Owned export control, retired with this snapshot. */
  guint64 revision;
  gboolean closed, invalidating, publishing;
} StatementUi;
/* Object retention and native window lifetime are different. Retained buttons
 * remain harmless after close, even when another application holds a reference. */
static StatementUi *StatementOwner(GtkWindow *window) {
  StatementUi *ui = g_object_get_data(G_OBJECT(window), STATEMENT_UI);
  return ui != NULL && !ui->closed && UmiGtk4WindowRegistered(window) ? ui : NULL;
}
static void StatementFree(gpointer data) {
  StatementUi *ui = data;
  UmiBankFeedDestroy(ui->feed);
  g_free(ui->account);
  g_free(ui->currency);
  g_clear_object(&ui->csv);
  g_clear_object(&ui->reviewArchive);
  g_clear_object(&ui->reviewSave);
  g_clear_object(&ui->dailyText);
  g_clear_object(&ui->dailyCsv);
  g_clear_object(&ui->dailySave);
  g_clear_object(&ui->save);
  g_free(ui);
}
static void StatementClosed(GtkWidget *window, gpointer unused) {
  (void)unused;
  StatementUi *ui = g_object_get_data(G_OBJECT(window), STATEMENT_UI);
  if (ui == NULL || ui->closed)
    return;
  ui->closed = TRUE;
  ++ui->revision;
  UmiBankFeedDestroy(ui->feed);
  ui->feed = NULL;
  UmiTextExportGtk4Retire(ui->save);
  UmiTextExportGtk4Retire(ui->dailySave);
  UmiTextExportGtk4Retire(ui->reviewSave);
}
static bool StatementCurrent(GtkWindow *window, guint64 revision) {
  StatementUi *ui = StatementOwner(window);
  return ui != NULL && ui->revision == revision;
}
/* Edits revoke the old export before they publish a message. Notification
 * handlers can close the window or edit another field, so recheck ownership. */
static void StatementInvalidate(GtkWindow *window) {
  StatementUi *ui = StatementOwner(window);
  if (ui == NULL)
    return;
  ++ui->revision;
  if (ui->invalidating)
    return;
  ui->invalidating = TRUE;
  /* The reusable review is authority for these exact fields. Revoke it before
   * any notification can try to save the previous comparison. */
  gtk_widget_set_sensitive(ui->reviewSave, FALSE);
  ui = StatementOwner(window);
  if (ui == NULL) return;
  gtk_text_buffer_set_text(ui->reviewArchive, "", 0);
  ui = StatementOwner(window);
  if (ui == NULL) return;
  gtk_widget_set_sensitive(ui->save, FALSE);
  ui = StatementOwner(window);
  if (ui == NULL)
    return;
  gtk_text_buffer_set_text(ui->csv, "", 0);
  ui = StatementOwner(window);
  if (ui == NULL)
    return;
  /* Daily and aggregate exports belong to the same reviewed input generation.
   * Revoke both before showing new assumptions or accepting another comparison. */
  gtk_widget_set_sensitive(ui->dailySave, FALSE);
  ui = StatementOwner(window);
  if (ui == NULL) return;
  gtk_text_buffer_set_text(ui->dailyCsv, "", 0);
  ui = StatementOwner(window);
  if (ui == NULL) return;
  gtk_text_buffer_set_text(ui->dailyText, "", 0);
  ui = StatementOwner(window);
  if (ui == NULL) return;
  gtk_label_set_text(GTK_LABEL(ui->result), "Enter dates and balances, then compare.");
  ui = StatementOwner(window);
  if (ui != NULL)
    ui->invalidating = FALSE;
}
static void StatementChanged(GtkEditable *editable, gpointer data) {
  (void)editable;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  StatementInvalidate(window);
  g_object_unref(window);
}
static UmiStatus StatementBalance(GtkWidget *entry, UmiDecimal *out) {
  const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
  /* Infer only the written fractional precision, never a currency conversion.
   * The common parser rejects exponent notation and excess fractional digits. */
  size_t length = strlen(text);
  const char *point = strchr(text, '.');
  size_t fractional = point == NULL ? 0U : strlen(point + 1);
  if (length > UMI_DECIMAL_TEXT_MAX || fractional > 9U)
    return UMI_STATUS_INVALID_ARGUMENT;
  return UmiDecimalParse(text, length, (uint8_t)fractional, out);
}
static void StatementLine(GString *text, const char *label, UmiDecimal value) {
  char number[UMI_DECIMAL_TEXT_MAX];
  if (UmiDecimalFormat(value, number, sizeof number) == UMI_STATUS_OK)
    g_string_append_printf(text, "%s: %s\n", label, number);
}
/* Format a detached report before touching any native widget. A notification
 * may close the host, so no imported feed is borrowed during publication. */
static GString *StatementDailyText(const UmiBankStatementTimeline *timeline, UmiStatus status) {
  GString *text = g_string_new("Daily booked balances\n");
  if (status != UMI_STATUS_OK) {
    g_string_append_printf(text, "Daily trace unavailable: %s. No partial balances are shown.\n", umi_status_text(status));
    return text;
  }
  UmiBankStatementTimelineSummary summary;
  (void)UmiBankStatementTimelineRead(timeline, &summary);
  StatementLine(text, "Minimum including supplied opening", summary.minimumBalance);
  g_string_append_printf(text, "Minimum date: %04d-%02u-%02u\n", (int)summary.minimumDate.year,
    (unsigned)summary.minimumDate.month, (unsigned)summary.minimumDate.day);
  if (summary.hasNegativeBalance)
    g_string_append_printf(text, "First negative opening or day close: %04d-%02u-%02u\n",
      (int)summary.firstNegativeDate.year, (unsigned)summary.firstNegativeDate.month, (unsigned)summary.firstNegativeDate.day);
  else g_string_append(text, "No negative opening or day close in the selected observations.\n");
  for (size_t i = 0U; i < summary.days; ++i) {
    UmiBankStatementDay day;
    (void)UmiBankStatementTimelineAt(timeline, i, &day);
    char credit[UMI_DECIMAL_TEXT_MAX], debit[UMI_DECIMAL_TEXT_MAX], balance[UMI_DECIMAL_TEXT_MAX];
    (void)UmiDecimalFormat(day.credits, credit, sizeof credit);
    (void)UmiDecimalFormat(day.debits, debit, sizeof debit);
    (void)UmiDecimalFormat(day.closing, balance, sizeof balance);
    g_string_append_printf(text, "%04d-%02u-%02u | credits %s | debits %s | closing %s | booked %zu | pending excluded %zu\n",
      (int)day.date.year, (unsigned)day.date.month, (unsigned)day.date.day, credit, debit, balance,
      day.bookedRows, day.pendingRows);
  }
  g_string_append(text, "Dates without observations keep the prior balance. Pending amounts are excluded.\n"
    "Provider calendar days; no intraday sequence, live balance, forecast or proof of completeness.\n");
  return text;
}
static void StatementCompare(GtkButton *button, gpointer data) {
  (void)button;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  StatementUi *ui = StatementOwner(window);
  if (ui == NULL || ui->invalidating || ui->publishing)
    goto finished;
  StatementInvalidate(window);
  ui = StatementOwner(window);
  if (ui == NULL)
    goto finished;
  const guint64 revision = ui->revision;
  UmiBankStatementRequest request = {0};
  request.account = ui->account;
  request.currency = ui->currency;
  UmiStatus status =
      UmiBankStatementDate(gtk_editable_get_text(GTK_EDITABLE(ui->first)), &request.firstDate);
  if (status == UMI_STATUS_OK)
    status = UmiBankStatementDate(gtk_editable_get_text(GTK_EDITABLE(ui->last)), &request.lastDate);
  if (status == UMI_STATUS_OK)
    status = StatementBalance(ui->opening, &request.openingBalance);
  if (status == UMI_STATUS_OK)
    status = StatementBalance(ui->closing, &request.closingBalance);
  UmiBankStatementResult result;
  if (status == UMI_STATUS_OK)
    status = UmiBankStatementReview(ui->feed, &request, &result);
  UmiCsvDocument *csv = NULL;
  UmiStatus exportStatus = status;
  if (status == UMI_STATUS_OK)
    exportStatus = UmiBankStatementExportCsv(ui->feed, &request, &csv);
  UmiBankStatementTimeline *timeline = NULL;
  UmiStatus dailyStatus = status;
  if (status == UMI_STATUS_OK)
    dailyStatus = UmiBankStatementTimelineCreate(ui->feed, &request, &timeline);
  GString *daily = StatementDailyText(timeline, dailyStatus);
  UmiCsvDocument *dailyCsv = NULL;
  if (dailyStatus == UMI_STATUS_OK) {
    UmiStatus dailyExportStatus = UmiBankStatementTimelineExportCsv(timeline, &dailyCsv);
    if (dailyExportStatus != UMI_STATUS_OK)
      g_string_append_printf(daily, "Daily CSV unavailable: %s.\n", umi_status_text(dailyExportStatus));
  }
  UmiBankStatementTimelineDestroy(timeline);
  UmiBankStatementArchive *review = NULL;
  char *reviewBytes = NULL;
  size_t reviewLength = 0U;
  UmiStatus reviewStatus = status;
  if (reviewStatus == UMI_STATUS_OK)
    reviewStatus = UmiBankStatementArchiveCreate(ui->feed, &request, &ui->source, &review);
  if (reviewStatus == UMI_STATUS_OK)
    reviewStatus = UmiBankStatementArchiveEncode(review, &reviewBytes, &reviewLength);
  UmiBankStatementArchiveDestroy(review);
  if (reviewStatus == UMI_STATUS_OK &&
      g_utf8_strlen(reviewBytes, (gssize)reviewLength) > UMI_TEXT_EXPORT_MAX_CHARACTERS)
    reviewStatus = UMI_STATUS_CAPACITY_EXCEEDED;
  GString *text = g_string_new("");
  if (status != UMI_STATUS_OK)
    g_string_append_printf(
        text, "Comparison unavailable: %s. Check YYYY-MM-DD dates and decimal balances.",
        umi_status_text(status));
  else {
    g_string_append(text, result.balancesMatch ? "Arithmetic matches the supplied balances.\n"
                                               : "The supplied closing balance differs.\n");
    StatementLine(text, "Booked credits", result.credits);
    StatementLine(text, "Booked debits", result.debits);
    StatementLine(text, "Net movement", result.netMovement);
    StatementLine(text, "Expected closing", result.expectedClosing);
    StatementLine(text, "Reported minus expected", result.difference);
    g_string_append_printf(
        text,
        "Booked: %zu; pending excluded: %zu; outside period: %zu\n"
        "Mutable booked: %zu; missing IDs: %zu; previous page: %s; next page: %s\n",
        result.bookedRows, result.pendingRows, result.outsidePeriodRows, result.mutableBookedRows,
        result.unidentifiedRows, result.hasPreviousPage ? "yes" : "no",
        result.hasNextPage ? "yes" : "no");
    g_string_append(text, "An arithmetic match does not prove a complete or authentic statement.");
    if (exportStatus != UMI_STATUS_OK)
      g_string_append(text, "\nCSV unavailable: the report could not fit in its bounded export.");
  }
  if (status == UMI_STATUS_OK) {
    if (reviewStatus == UMI_STATUS_OK)
      g_string_append(text, "\nA reusable review retains all imported accounts and these dates/balances. Save privately.");
    else
      g_string_append_printf(text, "\nReusable review unavailable: %s. The comparison above remains valid.",
          umi_status_text(reviewStatus));
  }
  /* Finish all work that borrows the feed before publishing any GTK changes.
   * Each notification may edit inputs; revision checks prevent stale exports. */
  ui->publishing = TRUE;
  gtk_label_set_text(GTK_LABEL(ui->result), text->str);
  if (StatementCurrent(window, revision) && csv != NULL) {
    ui = StatementOwner(window);
    gtk_text_buffer_set_text(ui->csv, UmiCsvDocumentData(csv), -1);
    if (StatementCurrent(window, revision)) {
      ui = StatementOwner(window);
      gtk_widget_set_sensitive(ui->save, TRUE);
    }
  }
  if (StatementCurrent(window, revision)) {
    ui = StatementOwner(window);
    gtk_text_buffer_set_text(ui->dailyText, daily->str, -1);
    if (StatementCurrent(window, revision) && dailyCsv != NULL) {
      ui = StatementOwner(window);
      gtk_text_buffer_set_text(ui->dailyCsv, UmiCsvDocumentData(dailyCsv), -1);
      if (StatementCurrent(window, revision)) {
        ui = StatementOwner(window);
        gtk_widget_set_sensitive(ui->dailySave, TRUE);
      }
    }
  }
  if (StatementCurrent(window, revision) && reviewStatus == UMI_STATUS_OK) {
    ui = StatementOwner(window);
    gtk_text_buffer_set_text(ui->reviewArchive, reviewBytes, (int)reviewLength);
    if (StatementCurrent(window, revision)) {
      ui = StatementOwner(window);
      gtk_widget_set_sensitive(ui->reviewSave, TRUE);
    }
  }
  UmiBankFeedArchiveFree(reviewBytes);
  /* A text-buffer notification can change inputs during set_text. Clear again
   * after the outer publication finishes, so no residual insertion stays live. */
  if (!StatementCurrent(window, revision) && StatementOwner(window) != NULL)
    StatementInvalidate(window);
  g_string_free(daily, TRUE);
  UmiCsvDocumentDestroy(dailyCsv);
  ui = StatementOwner(window);
  if (ui != NULL)
    ui->publishing = FALSE;
  g_string_free(text, TRUE);
  UmiCsvDocumentDestroy(csv);
finished:
  g_object_unref(window);
}
static GtkWidget *StatementEntry(GtkWidget *box, const char *label, const char *name) {
  GtkWidget *caption = gtk_label_new(label);
  gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
  gtk_box_append(GTK_BOX(box), caption);
  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_max_length(GTK_ENTRY(entry), UMI_DECIMAL_TEXT_MAX);
  gtk_widget_set_name(entry, name);
  gtk_box_append(GTK_BOX(box), entry);
  return entry;
}
/* The compatibility constructor below delegates to this provenance-aware
 * owner. Its previous signature is retained here for engineering review. */
#if 0
GtkWindow *UmiBankStatementGtk4Create(const UmiBankFeed *feed, const char *account,
                                      const char *currency, GtkWindow *parent) {
#endif
GtkWindow *UmiBankStatementGtk4CreateWithSource(const UmiBankFeed *feed, const char *account,
    const char *currency, const UmiBankFeedArchiveInfo *source, GtkWindow *parent) {
  if (parent != NULL && !UmiGtk4WindowRegistered(parent))
    return NULL;
  UmiBankFeedSummary summary;
  if (account == NULL || currency == NULL || UmiBankFeedRead(feed, &summary) != UMI_STATUS_OK)
    return NULL;
  bool found = false;
  for (size_t i = 0; i < summary.rows; ++i) {
    UmiBankFeedRow row;
    if (UmiBankFeedAt(feed, i, &row) == UMI_STATUS_OK && strcmp(row.account, account) == 0 &&
        strcmp(row.currency, currency) == 0) {
      found = true;
      break;
    }
  }
  if (!found)
    return NULL;
  /* Validate caller-supplied provenance through the canonical archive boundary
   * before constructing widgets. A digest remains an untrusted source claim. */
  /* Encoding the whole feed merely to validate provenance would reject a
   * usable comparison when its escaped archive exceeds the export bound.
   * Keep that earlier draft here; validate only the claim at this boundary. */
#if 0
  if (source != NULL) {
    char *validation = NULL;
    size_t validationLength = 0U;
    UmiStatus valid = UmiBankFeedArchiveEncode(feed, source, &validation, &validationLength);
    UmiBankFeedArchiveFree(validation);
    if (valid != UMI_STATUS_OK) return NULL;
  }
#endif
  if (source != NULL && UmiBankFeedArchiveInfoValidate(source) != UMI_STATUS_OK) return NULL;
  UmiBankFeed *copy = NULL;
  if (UmiBankFeedClone(feed, &copy) != UMI_STATUS_OK)
    return NULL;
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  StatementUi *ui = g_new0(StatementUi, 1);
  ui->feed = copy;
  if (source != NULL) ui->source = *source;
  ui->account = g_strdup(account);
  ui->currency = g_strdup(currency);
  g_object_set_data_full(G_OBJECT(window), STATEMENT_UI, ui, StatementFree);
  UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), StatementClosed, NULL);
  if (parent != NULL) {
    gtk_window_set_transient_for(window, parent);
    gtk_window_set_destroy_with_parent(window, TRUE);
    UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
  }
  gtk_window_set_title(window, "Statement balance comparison");
  gtk_window_set_default_size(window, 700, 680);
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_margin_start(box, 16);
  gtk_widget_set_margin_end(box, 16);
  gtk_widget_set_margin_top(box, 16);
  gtk_widget_set_margin_bottom(box, 16);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
  gtk_window_set_child(window, scroll);
  char *identity = g_strdup_printf(
      "Account %s | %s\nSnapshot from import; later source edits do not change this review.",
      account, currency);
  GtkWidget *intro = gtk_label_new(identity);
  g_free(identity);
  gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
  gtk_box_append(GTK_BOX(box), intro);
  ui->first = StatementEntry(box, "First booking date (YYYY-MM-DD)", "statement-first");
  ui->last = StatementEntry(box, "Last booking date (inclusive)", "statement-last");
  ui->opening =
      StatementEntry(box, "Balance immediately before the first date", "statement-opening");
  ui->closing = StatementEntry(box, "Reported balance after the last date", "statement-closing");
  GtkWidget *compare = gtk_button_new_with_label("Compare balances");
  gtk_widget_set_name(compare, "statement-compare");
  gtk_box_append(GTK_BOX(box), compare);
  ui->result = gtk_label_new("Enter dates and balances, then compare.");
  gtk_widget_set_name(ui->result, "statement-result");
  gtk_label_set_wrap(GTK_LABEL(ui->result), TRUE);
  gtk_label_set_selectable(GTK_LABEL(ui->result), TRUE);
  gtk_label_set_xalign(GTK_LABEL(ui->result), 0.0F);
  gtk_box_append(GTK_BOX(box), ui->result);
  ui->csv = gtk_text_buffer_new(NULL);
  ui->save = UmiTextExportGtk4CreateForBuffer(ui->csv, "statement-review.csv");
  gtk_widget_set_name(ui->save, "statement-export");
  gtk_widget_set_sensitive(ui->save, FALSE);
  gtk_box_append(GTK_BOX(box), ui->save);
  g_object_ref(ui->save);
  GtkWidget *dailyHeading = gtk_label_new("Daily balance trace and CSV");
  gtk_label_set_xalign(GTK_LABEL(dailyHeading), 0.0F);
  gtk_box_append(GTK_BOX(box), dailyHeading);
  ui->dailyText = gtk_text_buffer_new(NULL);
  GtkWidget *dailyView = gtk_text_view_new_with_buffer(ui->dailyText);
  gtk_widget_set_name(dailyView, "statement-daily-view");
  gtk_text_view_set_editable(GTK_TEXT_VIEW(dailyView), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(dailyView), TRUE);
  GtkWidget *dailyScroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(dailyScroll), dailyView);
  gtk_widget_set_size_request(dailyScroll, -1, 200);
  gtk_box_append(GTK_BOX(box), dailyScroll);
  ui->dailyCsv = gtk_text_buffer_new(NULL);
  ui->dailySave = UmiTextExportGtk4CreateForBuffer(ui->dailyCsv, "statement-daily-balances.csv");
  gtk_widget_set_name(ui->dailySave, "statement-daily-export");
  gtk_widget_set_sensitive(ui->dailySave, FALSE);
  gtk_box_append(GTK_BOX(box), ui->dailySave);
  g_object_ref(ui->dailySave);
  GtkWidget *archiveHint = gtk_label_new(
      "Save a reusable statement review, then reopen it in Bank transaction reports "
      "using Umicom statement review archive. It contains every imported account; "
      "source digests and supplied balances are not verified bank evidence.");
  gtk_label_set_wrap(GTK_LABEL(archiveHint), TRUE);
  gtk_box_append(GTK_BOX(box), archiveHint);
  ui->reviewArchive = gtk_text_buffer_new(NULL);
  ui->reviewSave = UmiTextExportGtk4CreateForBuffer(ui->reviewArchive, "statement-review.umibank.json");
  gtk_widget_set_name(ui->reviewSave, "statement-review-export");
  gtk_widget_set_sensitive(ui->reviewSave, FALSE);
  gtk_box_append(GTK_BOX(box), ui->reviewSave);
  g_object_ref(ui->reviewSave);
  GtkWidget *inputs[] = {ui->first, ui->last, ui->opening, ui->closing};
  for (size_t i = 0; i < 4U; ++i)
    g_signal_connect_object(inputs[i], "changed", G_CALLBACK(StatementChanged), window, 0);
  g_signal_connect_object(compare, "clicked", G_CALLBACK(StatementCompare), window, 0);
  return window;
}


/* Reopening supplies a copied scope to a new independent window. No saved
 * match flag is trusted: the native comparison uses the normal calculation. */
GtkWindow *UmiBankStatementGtk4CreateFromArchive(const UmiBankStatementArchive *archive,
    GtkWindow *parent) {
  UmiBankStatementArchiveSnapshot scope;
  UmiBankFeed *feed = NULL;
  if (UmiBankStatementArchiveRead(archive, &scope) != UMI_STATUS_OK ||
      UmiBankStatementArchiveCloneFeed(archive, &feed) != UMI_STATUS_OK) return NULL;
  GtkWindow *window = UmiBankStatementGtk4CreateWithSource(feed, scope.account, scope.currency,
      &scope.source, parent);
  UmiBankFeedDestroy(feed);
  if (window == NULL) return NULL;
  g_object_ref(window);
  char first[32], last[32], opening[UMI_DECIMAL_TEXT_MAX], closing[UMI_DECIMAL_TEXT_MAX];
  (void)snprintf(first, sizeof first, "%04d-%02u-%02u", (int)scope.firstDate.year,
      (unsigned)scope.firstDate.month, (unsigned)scope.firstDate.day);
  (void)snprintf(last, sizeof last, "%04d-%02u-%02u", (int)scope.lastDate.year,
      (unsigned)scope.lastDate.month, (unsigned)scope.lastDate.day);
  UmiStatus status = UmiDecimalFormat(scope.openingBalance, opening, sizeof opening);
  if (status == UMI_STATUS_OK) status = UmiDecimalFormat(scope.closingBalance, closing, sizeof closing);
  const char *values[] = {first, last, opening, closing};
  for (size_t i = 0U; status == UMI_STATUS_OK && i < 4U; ++i) {
    StatementUi *ui = StatementOwner(window);
    if (ui == NULL) { status = UMI_STATUS_INVALID_STATE; break; }
    GtkWidget *fields[] = {ui->first, ui->last, ui->opening, ui->closing};
    gtk_editable_set_text(GTK_EDITABLE(fields[i]), values[i]);
  }
  if (status == UMI_STATUS_OK && StatementOwner(window) != NULL)
    StatementCompare(NULL, window);
  bool open = status == UMI_STATUS_OK && StatementOwner(window) != NULL;
  if (!open) gtk_window_destroy(window);
  g_object_unref(window);
  return open ? window : NULL;
}
GtkWindow *UmiBankStatementGtk4Create(const UmiBankFeed *feed, const char *account,
    const char *currency, GtkWindow *parent) {
  return UmiBankStatementGtk4CreateWithSource(feed, account, currency, NULL, parent);
}
