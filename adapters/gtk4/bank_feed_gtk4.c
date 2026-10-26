/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_feed_gtk4.c
 *
 * PURPOSE:
 *   Keep private bank file loading asynchronous and retire results when their window closes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/bank_feed/transactions.h"
#include "umicom/bank_feed/archive.h"
#include "umicom/base/sha256.h"
#include "umicom/bank_feed/csv_import.h"
#include "umicom/bank_feed/csv_mapping.h"
#include "umicom/platform/input_file.h"
#include "umicom/ui/gtk4/bank_feed.h"
#include "umicom/bank_feed/statement.h"
#include "umicom/ui/gtk4/bank_comparison.h"
#include "umicom/ui/gtk4/bank_statement.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/ui/gtk4/text_export.h"
#include "window_removal_private.h"
#include <string.h>
#define BANK_FEED_UI "umicom-bank-feed-window"
typedef struct BankFeedUi {
  GtkWindow *window;
  GtkWidget *path, *groups, *view, *note, *save;
  GtkWidget *format;
  GtkWidget *schema;
  GtkWidget *mapping, *mapHeaders[UMI_BANK_CSV_FIELD_COUNT], *mapAccount, *mapCurrency;
  GtkWidget *mapDate, *mapAmount, *mapStatus, *mapMutability;
  GtkTextBuffer *csv;
  GtkTextBuffer *archive;
  GtkWidget *archiveSave, *archiveNote;
  UmiBankFeedArchiveInfo archiveInfo;
  UmiBankFeed *feed;
  UmiBankFeed *comparisonBefore; /* Explicit independent snapshot, not an implicit history. */
  UmiBankFeedArchiveInfo comparisonInfo;
  GtkWidget *comparisonNote;
  size_t groupRows[UMI_BANK_FEED_MAX_ROWS], groupCount;
  guint64 generation;
  gboolean closed, publishing, loading, invalidating;
} BankFeedUi;
typedef struct BankFeedLoad {
  GWeakRef window;
  char *path;
  guint64 generation;
  UmiBankFeed *feed;
  UmiStatus status;
  guint format;
  UmiBankFeedCsvIssue issue;
  UmiBankFeedArchiveInfo archiveInfo;
  UmiBankFeedCsvMapping mapping;
  UmiBankStatementArchive *review; /* Worker-owned until native completion. */
} BankFeedLoad;
/* Retaining a GTK object does not keep its native window open. Every control
 * callback checks both ownership and native registration before using children. */
static BankFeedUi *FeedOwner(GtkWindow *window) {
  BankFeedUi *ui = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
  return ui != NULL && !ui->closed && UmiGtk4WindowRegistered(window) ? ui : NULL;
}
static void FeedFree(gpointer data) {
  BankFeedUi *ui = data;
  UmiBankFeedDestroy(ui->feed);
  UmiBankFeedDestroy(ui->comparisonBefore);
  g_clear_object(&ui->csv);
  g_clear_object(&ui->archive);
  g_clear_object(&ui->archiveSave);
  g_clear_object(&ui->save);
  g_free(ui);
}
/* The task owns its filename and result. A weak window reference lets the
 * task finish safely after the user has closed the inspector. */
static void FeedLoadFree(gpointer data) {
  BankFeedLoad *load = data;
  g_weak_ref_clear(&load->window);
  UmiBankFeedDestroy(load->feed);
  UmiBankStatementArchiveDestroy(load->review);
  g_free(load->path);
  g_free(load);
}
static void FeedClosed(GtkWidget *window, gpointer unused) {
  (void)unused;
  BankFeedUi *ui = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
  if (ui == NULL || ui->closed)
    return;
  ui->closed = TRUE;
  ++ui->generation;
  UmiBankFeedDestroy(ui->feed);
  ui->feed = NULL;
  UmiBankFeedDestroy(ui->comparisonBefore);
  ui->comparisonBefore = NULL;
  /* Export may retain an already accepted immutable write. Retirement blocks
   * new captures without claiming that closing undoes a requested save. */
  UmiTextExportGtk4Retire(ui->save);
  UmiTextExportGtk4Retire(ui->archiveSave);
}
/* File reading and parsing run away from the UI thread. Only the completion
 * callback may publish into GTK, and it must still match the original request. */
static void FeedWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  BankFeedLoad *load = data;
  unsigned char *bytes = NULL;
  size_t length = 0U;
  /* Only the reviewed envelope needs its small scope allowance. Existing
   * transaction formats keep their previous input bound. */
#if 0
  load->status = UmiInputFileRead(load->path, UMI_BANK_FEED_MAX_BYTES, &bytes, &length);
#endif
  size_t limit = load->format == 4U ? UMI_BANK_STATEMENT_ARCHIVE_MAX_BYTES : UMI_BANK_FEED_MAX_BYTES;
  load->status = UmiInputFileRead(load->path, limit, &bytes, &length);
  /* Preserve the JSON-only dispatch for review. Explicit format selection
   * replaces it so a filename extension never decides financial semantics. */
#if 0
  if (load->status == UMI_STATUS_OK)
    load->status = UmiBankFeedCreate(bytes, length, &load->feed);
#endif
  if (load->status == UMI_STATUS_OK) {
    if (load->format == 0U)
      load->status = UmiBankFeedCreate(bytes, length, &load->feed);
    else if (load->format == 1U)
      load->status = UmiBankFeedCsvCreate(bytes, length, &load->feed, &load->issue);
    else if (load->format == 2U)
      load->status = UmiBankFeedCsvMappedCreate(bytes, length, &load->mapping,
                                                &load->feed, &load->issue);
    else if (load->format == 3U)
      load->status = UmiBankFeedArchiveDecode(bytes, length, &load->feed, &load->archiveInfo);
    else if (load->format == 4U) {
      load->status = UmiBankStatementArchiveDecode(bytes, length, &load->review);
      UmiBankStatementArchiveSnapshot snapshot;
      if (load->status == UMI_STATUS_OK)
        load->status = UmiBankStatementArchiveRead(load->review, &snapshot);
      if (load->status == UMI_STATUS_OK) {
        load->archiveInfo = snapshot.source;
        load->status = UmiBankStatementArchiveCloneFeed(load->review, &load->feed);
      }
    }
    else
      load->status = UMI_STATUS_INVALID_ARGUMENT;
  }
  /* Preserve original input identity without retaining its private filename.
   * Reopened archives retain their claimed source digest; do not relabel an
   * archive hash as if it were the original provider response. */
  if (load->status == UMI_STATUS_OK && load->format < 3U) {
    load->archiveInfo.sourceFormat = (UmiBankFeedSourceFormat)(load->format + 1U);
    load->status = UmiSha256Buffer(bytes, length, load->archiveInfo.sourceSha256);
  }
  UmiInputFileFree(bytes);
  g_task_return_boolean(task, TRUE);
}
static void FeedDecimalLine(GString *text, const char *label, UmiDecimal value) {
  char number[96];
  if (UmiDecimalFormat(value, number, sizeof number) == UMI_STATUS_OK)
    g_string_append_printf(text, "%s: %s\n", label, number);
}
static void FeedRender(GtkWindow *window) {
  BankFeedUi *ui = FeedOwner(window);
  if (ui == NULL || ui->publishing || ui->feed == NULL)
    return;
  guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->groups));
  GString *text =
      g_string_new("Imported bank data. Authenticity and statement completeness are unverified.\n");
  const char *sourceNames[] = {"unknown", "Open Banking JSON", "statement CSV", "mapped CSV"};
  g_string_append_printf(text, "Original source format: %s\nOriginal input SHA-256: %s\n"
      "A restored digest is a retained claim, not authenticated provider evidence.\n\n",
      sourceNames[ui->archiveInfo.sourceFormat],
      ui->archiveInfo.sourceSha256[0] ? ui->archiveInfo.sourceSha256 : "not retained");
  UmiBankFeedSummary page;
  (void)UmiBankFeedRead(ui->feed, &page);
  g_string_append_printf(text,
                         "Page rows: %zu; missing IDs: %zu; previous page: %s; next page: %s\n\n",
                         page.rows, page.missingIdentifiers, page.hasPreviousPage ? "yes" : "no",
                         page.hasNextPage ? "yes" : "no");
  if ((size_t)index < ui->groupCount) {
    UmiBankFeedRow selected;
    (void)UmiBankFeedAt(ui->feed, ui->groupRows[index], &selected);
    g_string_append_printf(text, "Account %s | %s\n", selected.account, selected.currency);
    UmiBankFeedTotals totals;
    UmiStatus status = UmiBankFeedSum(ui->feed, selected.account, selected.currency, &totals);
    if (status == UMI_STATUS_OK) {
      FeedDecimalLine(text, "Booked credits", totals.bookedCredits);
      FeedDecimalLine(text, "Booked debits", totals.bookedDebits);
      FeedDecimalLine(text, "Booked net movement", totals.bookedNet);
      FeedDecimalLine(text, "Pending credits", totals.pendingCredits);
      FeedDecimalLine(text, "Pending debits", totals.pendingDebits);
      FeedDecimalLine(text, "Pending net movement", totals.pendingNet);
      g_string_append_printf(text, "Mutable rows: %zu; unidentified rows: %zu\n",
                             totals.mutableRows, totals.unidentifiedRows);
    } else
      g_string_append_printf(text, "Totals unavailable: %s\n", umi_status_text(status));
    g_string_append(text, "Net movement is not an account balance. Rows retain source order.\n\n");
    for (size_t i = 0; i < page.rows; ++i) {
      UmiBankFeedRow row;
      (void)UmiBankFeedAt(ui->feed, i, &row);
      if (strcmp(row.account, selected.account) != 0 ||
          strcmp(row.currency, selected.currency) != 0)
        continue;
      g_string_append_printf(text, "%s | %s | %s %s | %s | %s\n  ID: %s | %s\n", row.bookingTime,
                             row.booked ? "Booked" : "Pending", row.credit ? "Credit" : "Debit",
                             row.amountText, row.mutableRecord ? "Mutable" : "Immutable",
                             row.information, row.transaction[0] ? row.transaction : "not supplied",
                             row.reference);
    }
  }
  /* Compose before touching GTK: a changed signal may close or clear the
   * window. No model pointer is used after publishing the completed text. */
  ui->publishing = TRUE;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->view));
  gtk_text_buffer_set_text(buffer, text->str, -1);
  ui = FeedOwner(window);
  if (ui != NULL)
    ui->publishing = FALSE;
  g_string_free(text, TRUE);
}
static void FeedSelection(GObject *object, GParamSpec *spec, gpointer data) {
  (void)object;
  (void)spec;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  FeedRender(window);
  g_object_unref(window);
}
static void FeedInvalidate(GtkWindow *window) {
  BankFeedUi *ui = FeedOwner(window);
  if (ui == NULL || ui->invalidating)
    return;
  ui->invalidating = TRUE;
  ++ui->generation;
  UmiBankFeedDestroy(ui->feed);
  ui->feed = NULL;
  ui->groupCount = 0U;
  memset(&ui->archiveInfo, 0, sizeof ui->archiveInfo);
  /* Release owned data before notifications, then recheck the window after
   * each call that can invoke application handlers. */
  gtk_text_buffer_set_text(ui->csv, "", 0);
  ui = FeedOwner(window);
  if (ui == NULL)
    return;
  gtk_widget_set_sensitive(ui->save, FALSE);
  ui = FeedOwner(window);
  if (ui == NULL)
    return;
  gtk_widget_set_sensitive(ui->archiveSave, FALSE);
  ui = FeedOwner(window);
  if (ui == NULL) return;
  gtk_text_buffer_set_text(ui->archive, "", 0);
  ui = FeedOwner(window);
  if (ui == NULL) return;
  gtk_label_set_text(GTK_LABEL(ui->archiveNote), "No reusable snapshot is available until import succeeds.");
  ui = FeedOwner(window);
  if (ui == NULL) return;
  gtk_drop_down_set_model(GTK_DROP_DOWN(ui->groups), NULL);
  ui = FeedOwner(window);
  if (ui == NULL)
    return;
  gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->view)), "", 0);
  ui = FeedOwner(window);
  if (ui == NULL)
    return;
  /* The earlier prompt is retained because it describes the existing JSON
   * workflow; the replacement also covers the explicit CSV import path. */
#if 0
  gtk_label_set_text(GTK_LABEL(ui->note), "Choose a saved Open Banking transaction response.");
#endif
  gtk_label_set_text(GTK_LABEL(ui->note), "Choose the file format and a saved transaction file.");
  ui = FeedOwner(window);
  if (ui != NULL)
    ui->invalidating = FALSE;
}
static void FeedChanged(GtkEditable *editable, gpointer data) {
  (void)editable;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  FeedInvalidate(window);
  g_object_unref(window);
}
/* Format belongs to the same request generation as the path. Changing either
 * retires the previous report and any pending worker result before reuse. */
static void FeedFormatChanged(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  FeedInvalidate(window);
  BankFeedUi *ui = FeedOwner(window);
  if (ui != NULL && ui->mapping != NULL)
    gtk_widget_set_visible(ui->mapping,
        gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->format)) == 2U);
  ui = FeedOwner(window);
  if (ui != NULL && ui->schema != NULL)
    gtk_widget_set_visible(ui->schema,
        gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->format)) == 1U);
  g_object_unref(window);
}
static void FeedClear(GtkButton *button, gpointer data) {
  (void)button;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  FeedInvalidate(window);
  g_object_unref(window);
}
static bool FeedCurrent(GtkWindow *window, guint64 generation) {
  BankFeedUi *ui = FeedOwner(window);
  return ui != NULL && ui->generation == generation;
}
/* Publish on the GTK thread. Every notifying GTK operation may invoke an
 * application callback, so recheck the request after each such operation. */
static void FeedLoaded(GObject *source, GAsyncResult *result, gpointer unused) {
  (void)source;
  (void)unused;
  BankFeedLoad *load = g_task_get_task_data(G_TASK(result));
  GtkWindow *window = g_weak_ref_get(&load->window);
  if (window == NULL)
    return;
  BankFeedUi *ui = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
  if (ui != NULL)
    ui->loading = FALSE;
  if (!FeedCurrent(window, load->generation)) {
    g_object_unref(window);
    return;
  }
  if (load->status != UMI_STATUS_OK) {
    char *message = g_strdup_printf("Import failed: %s. The previous report has been cleared.",
                                    umi_status_text(load->status));
    /* Error locations describe the input record, without echoing private
     * account identifiers or descriptions into diagnostics. */
    if (load->issue.record != 0U) {
      char *located = g_strdup_printf("%s CSV record %zu, column %zu (0 means the whole record).",
                                     message, load->issue.record, load->issue.column);
      g_free(message);
      message = located;
    }
    gtk_label_set_text(GTK_LABEL(ui->note), message);
    g_free(message);
    g_object_unref(window);
    return;
  }
  ui->archiveInfo = load->archiveInfo;
  ui->feed = load->feed;
  load->feed = NULL;
  UmiBankFeedSummary summary;
  (void)UmiBankFeedRead(ui->feed, &summary);
  GtkStringList *groups = gtk_string_list_new(NULL);
  for (size_t i = 0; i < summary.rows; ++i) {
    UmiBankFeedRow row;
    (void)UmiBankFeedAt(ui->feed, i, &row);
    bool seen = false;
    for (size_t j = 0; j < ui->groupCount; ++j) {
      UmiBankFeedRow prior;
      (void)UmiBankFeedAt(ui->feed, ui->groupRows[j], &prior);
      if (strcmp(prior.account, row.account) == 0 && strcmp(prior.currency, row.currency) == 0) {
        seen = true;
        break;
      }
    }
    if (!seen) {
      ui->groupRows[ui->groupCount++] = i;
      char *label = g_strdup_printf("%s | %s", row.account, row.currency);
      gtk_string_list_append(groups, label);
      g_free(label);
    }
  }
  UmiCsvDocument *csv = NULL;
  UmiStatus exportStatus = UmiBankFeedExportCsv(ui->feed, &csv);
  char *archive = NULL;
  size_t archiveLength = 0U;
  UmiStatus archiveStatus = UmiBankFeedArchiveEncode(ui->feed, &ui->archiveInfo, &archive, &archiveLength);
  if (archiveStatus == UMI_STATUS_OK &&
      g_utf8_strlen(archive, (gssize)archiveLength) > UMI_TEXT_EXPORT_MAX_CHARACTERS)
    archiveStatus = UMI_STATUS_CAPACITY_EXCEEDED;
  ui->publishing = TRUE;
  gtk_drop_down_set_model(GTK_DROP_DOWN(ui->groups), G_LIST_MODEL(groups));
  g_object_unref(groups);
  if (!FeedCurrent(window, load->generation))
    goto finished;
  ui = FeedOwner(window);
  gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->groups),
                             summary.rows ? 0U : GTK_INVALID_LIST_POSITION);
  if (!FeedCurrent(window, load->generation))
    goto finished;
  ui = FeedOwner(window);
  if (exportStatus == UMI_STATUS_OK)
    gtk_text_buffer_set_text(ui->csv, UmiCsvDocumentData(csv), -1);
  if (!FeedCurrent(window, load->generation))
    goto finished;
  ui = FeedOwner(window);
  gtk_widget_set_sensitive(ui->save, exportStatus == UMI_STATUS_OK);
  if (!FeedCurrent(window, load->generation))
    goto finished;
  ui = FeedOwner(window);
  if (archiveStatus == UMI_STATUS_OK)
    gtk_text_buffer_set_text(ui->archive, archive, (int)archiveLength);
  if (!FeedCurrent(window, load->generation)) goto finished;
  ui = FeedOwner(window);
  gtk_widget_set_sensitive(ui->archiveSave, archiveStatus == UMI_STATUS_OK);
  if (!FeedCurrent(window, load->generation)) goto finished;
  ui = FeedOwner(window);
  gtk_label_set_text(GTK_LABEL(ui->archiveNote), archiveStatus == UMI_STATUS_OK
      ? "Save privately, then reopen with Umicom observation archive. All imported accounts are included."
      : "Reusable snapshot unavailable: the encoded report exceeds its bounded export or could not be allocated. No partial snapshot is offered.");
  if (!FeedCurrent(window, load->generation)) goto finished;
  ui = FeedOwner(window);
  ui->publishing = FALSE;
  gtk_label_set_text(
      GTK_LABEL(ui->note),
      exportStatus == UMI_STATUS_OK
          ? "Imported locally. The CSV export includes all accounts in this page. Save it "
            "privately."
          : "Imported locally; CSV exceeds the export limit. No partial CSV is available.");
  if (FeedCurrent(window, load->generation))
    FeedRender(window);
  /* The worker decoded and copied the scope. Only a still-current request may
   * open its editor; changing format/path or closing the parent retires it. */
  if (load->review != NULL && FeedCurrent(window, load->generation)) {
    GtkWindow *review = UmiBankStatementGtk4CreateFromArchive(load->review, window);
    if (review != NULL) {
      if (FeedCurrent(window, load->generation) && UmiGtk4WindowRegistered(review))
        gtk_window_present(review);
      else gtk_window_destroy(review);
    } else if (FeedCurrent(window, load->generation)) {
      ui = FeedOwner(window);
      gtk_label_set_text(GTK_LABEL(ui->note),
          "Observations imported, but the saved comparison window could not be opened.");
    }
  }
finished:
  ui = FeedOwner(window);
  if (ui != NULL)
    ui->publishing = FALSE;
  /* A text insertion may finish after a reentrant invalidation. Clear its
   * residue only while no newer load or published feed owns the controls. */
  if (ui != NULL && ui->feed == NULL && !ui->loading &&
      ui->generation != load->generation) FeedInvalidate(window);
  UmiBankFeedArchiveFree(archive);
  UmiCsvDocumentDestroy(csv);
  g_object_unref(window);
}
#include "bank_feed_mapping.inc"
static void FeedLoadClicked(GtkButton *button, gpointer data) {
  (void)button;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  BankFeedUi *ui = FeedOwner(window);
  if (ui == NULL || ui->publishing || ui->loading || ui->invalidating) {
    g_object_unref(window);
    return;
  }
  /* At most one bounded import worker belongs to a window. Path changes
   * invalidate its result, and a later click can load again after completion. */
  ui->loading = TRUE;
  guint64 generation = ui->generation + 1U;
  char *path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(ui->path)));
  guint format = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->format));
  UmiBankFeedCsvMapping mapping = {0};
  if (format == 2U && !FeedMappingRead(ui, &mapping)) {
    ui->loading = FALSE;
    gtk_label_set_text(GTK_LABEL(ui->note),
        "A mapping name or constant is too long, or a format choice is missing.");
    g_free(path);
    g_object_unref(window);
    return;
  }
  FeedInvalidate(window);
  ui = FeedOwner(window);
  /* Resetting controls can invoke application callbacks. The selected file
   * must still match this click before the worker receives an owned path. */
  /* The previous guard remains for review; the active guard also binds
   * the parser choice to the captured path and request generation. */
#if 0
  if (ui == NULL || ui->generation != generation ||
      strcmp(path, gtk_editable_get_text(GTK_EDITABLE(ui->path))) != 0) {
#endif
  /* Retain the two-format guard for review. Mapped imports additionally bind
   * every column and interpretation setting to this request. */
#if 0
  if (ui == NULL || ui->generation != generation ||
      strcmp(path, gtk_editable_get_text(GTK_EDITABLE(ui->path))) != 0 ||
      format != gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->format))) {
#endif
  if (ui == NULL || ui->generation != generation ||
      strcmp(path, gtk_editable_get_text(GTK_EDITABLE(ui->path))) != 0 ||
      format != gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->format)) ||
      (format == 2U && !FeedMappingMatches(ui, &mapping))) {
    BankFeedUi *retained = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
    if (retained != NULL)
      retained->loading = FALSE;
    g_free(path);
    g_object_unref(window);
    return;
  }
  BankFeedLoad *load = g_new0(BankFeedLoad, 1);
  g_weak_ref_init(&load->window, window);
  load->path = path;
  load->format = format;
  load->mapping = mapping;
  load->generation = ui->generation;
  gtk_label_set_text(GTK_LABEL(ui->note), "Reading selected file...");
  if (FeedCurrent(window, load->generation)) {
    GTask *task = g_task_new(NULL, NULL, FeedLoaded, NULL);
    g_task_set_task_data(task, load, FeedLoadFree);
    g_task_run_in_thread(task, FeedWorker);
    g_object_unref(task);
  } else {
    ui = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
    if (ui != NULL)
      ui->loading = FALSE;
    FeedLoadFree(load);
  }
  g_object_unref(window);
}
/* A separate owned snapshot prevents later imports or account selections from
 * changing the evidence behind an open comparison. Framework owns both windows;
 * applications only need to expose the existing transaction-report launcher. */
static void FeedStatementClicked(GtkButton *button, gpointer data) {
  (void)button;
  GtkWindow *window = g_object_ref(GTK_WINDOW(data));
  BankFeedUi *ui = FeedOwner(window);
  if (ui != NULL && !ui->publishing && !ui->invalidating && ui->feed != NULL) {
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->groups));
    if ((size_t)index < ui->groupCount) {
      UmiBankFeedRow selected;
      if (UmiBankFeedAt(ui->feed, ui->groupRows[index], &selected) == UMI_STATUS_OK) {
        /* Keep the previous constructor call for review. The source claim now
         * travels into the owned comparison and its reusable private export. */
#if 0
        GtkWindow *review = UmiBankStatementGtk4Create(ui->feed, selected.account, selected.currency, window);
#endif
        GtkWindow *review = UmiBankStatementGtk4CreateWithSource(ui->feed, selected.account,
            selected.currency, &ui->archiveInfo, window);
        if (review != NULL)
          gtk_window_present(review);
      }
    }
  }
  g_object_unref(window);
}
static GtkWidget *FeedNamed(GtkWidget *widget, const char *name) {
  gtk_widget_set_name(widget, name);
  return widget;
}
#include "bank_feed_comparison.inc"

GtkWindow *UmiBankFeedGtk4Create(GtkWindow *parent) {
  if (parent != NULL && !UmiGtk4WindowRegistered(parent))
    return NULL;
  GtkWindow *window = GTK_WINDOW(gtk_window_new());
  BankFeedUi *ui = g_new0(BankFeedUi, 1);
  ui->window = window;
  g_object_set_data_full(G_OBJECT(window), BANK_FEED_UI, ui, FeedFree);
  UmiGtk4ObserveWindowRemoval(window, G_OBJECT(window), FeedClosed, NULL);
  gtk_window_set_title(window, "Bank transaction reports");
  gtk_window_set_default_size(window, 1000, 720);
  if (parent != NULL) {
    gtk_window_set_transient_for(window, parent);
    gtk_window_set_destroy_with_parent(window, TRUE);
    UmiGtk4ObserveWindowRemoval(parent, G_OBJECT(window), UmiGtk4CloseRemovedParentChild, window);
  }
  GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_margin_start(root, 16);
  gtk_widget_set_margin_end(root, 16);
  gtk_widget_set_margin_top(root, 16);
  gtk_widget_set_margin_bottom(root, 16);
  /* Retain the former fixed form for review. Whole-form scrolling keeps
   * mapping and archive controls reachable in a small native window. */
#if 0
  gtk_window_set_child(window, root);
#endif
  GtkWidget *formScroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(formScroll), root);
  gtk_window_set_child(window, formScroll);
  /* Retain the JSON introduction for review. The shared window now offers
   * a second local format while keeping broker authentication separate. */
#if 0
  GtkWidget *intro = gtk_label_new("Review a saved Open Banking JSON response. This imports data "
                                   "only; it does not connect to a bank.");
#endif
  /* Retain the earlier CSV introduction for review. Custom mappings now let
   * users describe their source headers before the same local validation. */
#if 0
  GtkWidget *intro = gtk_label_new("Review a saved Open Banking JSON response or a statement CSV "
                                   "prepared in the format below. This imports data only; it does "
                                   "not connect to a bank.");
#endif
  GtkWidget *intro = gtk_label_new("Review a saved transaction file. Choose Open Banking JSON, "
                                   "the standard statement CSV, or CSV with your own column names. "
                                   "Importing does not connect to a bank or post transactions.");
  gtk_label_set_wrap(GTK_LABEL(intro), TRUE);
  gtk_box_append(GTK_BOX(root), intro);
  /* Named, explicit choices are keyboard accessible and copied into each
   * worker. Future formats need their own documented parser and regression cases. */
  /* Keep the original choices for review; custom mapping extends the same
   * import boundary without changing the JSON or normalised CSV choices. */
#if 0
  const char *formats[] = {"Open Banking JSON", "Statement CSV", NULL};
#endif
  /* Retain the prior choices for review. Explicit archive selection restores
   * retained observations; it does not reinterpret an arbitrary JSON response. */
#if 0
  const char *formats[] = {"Open Banking JSON", "Statement CSV", "CSV with custom columns", NULL};
#endif
  /* Append the review format without changing existing stored choice indexes.
   * The earlier four-choice catalogue remains for engineering review. */
#if 0
  const char *formats[] = {"Open Banking JSON", "Statement CSV", "CSV with custom columns",
                           "Umicom observation archive", NULL};
#endif
  const char *formats[] = {"Open Banking JSON", "Statement CSV", "CSV with custom columns",
                           "Umicom observation archive", "Umicom statement review archive", NULL};
  ui->format = FeedNamed(gtk_drop_down_new_from_strings(formats), "bank-feed-format");
  gtk_widget_set_tooltip_text(ui->format, "Select the format of the file you are importing.");
  gtk_box_append(GTK_BOX(root), ui->format);
  ui->mapping = FeedMappingCreate(ui);
  gtk_box_append(GTK_BOX(root), ui->mapping);
  GtkWidget *schema = gtk_label_new(
      "CSV columns: account,transaction,reference,information,booking_date,currency,amount,status,"
      "mutability. Date: YYYY-MM-DD. Negative amounts are debits. Status: BOOK or PDNG. "
      "Mutability: Immutable or Mutable.");
  gtk_label_set_wrap(GTK_LABEL(schema), TRUE);
  gtk_box_append(GTK_BOX(root), schema);
  ui->schema = schema;
  gtk_widget_set_visible(schema, FALSE);
  ui->path = FeedNamed(gtk_entry_new(), "bank-feed-path");
  /* Keep the original JSON hint for source review. Both supported formats
   * use the same explicit, user-selected file location. */
#if 0
  gtk_entry_set_placeholder_text(GTK_ENTRY(ui->path),
                                 "Absolute path to a bank transaction JSON file");
#endif
  gtk_entry_set_placeholder_text(GTK_ENTRY(ui->path),
                                 "Absolute path to a bank transaction JSON or statement CSV file");
  gtk_box_append(GTK_BOX(root), ui->path);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(root), actions);
  GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(ui->path), UMI_PATH_PICKER_EXISTING_FILE,
                                              "Choose a bank transaction response", NULL);
  if (browse != NULL)
    gtk_box_append(GTK_BOX(actions), browse);
  GtkWidget *load = FeedNamed(gtk_button_new_with_label("Import for review"), "bank-feed-load");
  GtkWidget *clear = FeedNamed(gtk_button_new_with_label("Clear report"), "bank-feed-clear");
  gtk_box_append(GTK_BOX(actions), load);
  gtk_box_append(GTK_BOX(actions), clear);
  ui->groups = FeedNamed(gtk_drop_down_new(NULL, NULL), "bank-feed-groups");
  gtk_box_append(GTK_BOX(root), ui->groups);
  ui->note = FeedNamed(gtk_label_new("No bank data loaded."), "bank-feed-note");
  gtk_label_set_wrap(GTK_LABEL(ui->note), TRUE);
  gtk_box_append(GTK_BOX(root), ui->note);
  ui->view = FeedNamed(gtk_text_view_new(), "bank-feed-output");
  gtk_text_view_set_editable(GTK_TEXT_VIEW(ui->view), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(ui->view), TRUE);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), ui->view);
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_widget_set_size_request(scroll, -1, 180);
  gtk_box_append(GTK_BOX(root), scroll);
  ui->csv = gtk_text_buffer_new(NULL);
  ui->save = UmiTextExportGtk4CreateForBuffer(ui->csv, "bank-transactions.csv");
  gtk_widget_set_name(ui->save, "bank-feed-export");
  gtk_widget_set_sensitive(ui->save, FALSE);
  gtk_box_append(GTK_BOX(root), ui->save);
  /* Keep the exporter alive for retirement even if GTK removes child ownership
   * before a retained native window is finally released. */
  g_object_ref(ui->save);
  GtkWidget *archiveLabel = gtk_label_new("Reusable observation archive (all accounts in this import)");
  gtk_label_set_wrap(GTK_LABEL(archiveLabel), TRUE);
  gtk_box_append(GTK_BOX(root), archiveLabel);
  ui->archiveNote = gtk_label_new("No reusable snapshot is available until import succeeds.");
  gtk_label_set_wrap(GTK_LABEL(ui->archiveNote), TRUE);
  gtk_widget_set_name(ui->archiveNote, "bank-feed-archive-note");
  gtk_box_append(GTK_BOX(root), ui->archiveNote);
  ui->archive = gtk_text_buffer_new(NULL);
  ui->archiveSave = UmiTextExportGtk4CreateForBuffer(ui->archive, "bank-observations.umibank.json");
  gtk_widget_set_name(ui->archiveSave, "bank-feed-archive-export");
  gtk_widget_set_sensitive(ui->archiveSave, FALSE);
  gtk_widget_set_tooltip_text(ui->archiveSave,
      "Save a private snapshot for reopening with Umicom observation archive. "
      "This retains imported rows, not a verified bank statement or ledger backup.");
  gtk_box_append(GTK_BOX(root), ui->archiveSave);
  g_object_ref(ui->archiveSave);
  g_signal_connect_object(ui->path, "changed", G_CALLBACK(FeedChanged), window, 0);
  g_signal_connect_object(ui->format, "notify::selected", G_CALLBACK(FeedFormatChanged), window, 0);
  g_signal_connect_object(load, "clicked", G_CALLBACK(FeedLoadClicked), window, 0);
  g_signal_connect_object(clear, "clicked", G_CALLBACK(FeedClear), window, 0);
  g_signal_connect_object(ui->groups, "notify::selected", G_CALLBACK(FeedSelection), window, 0);
  GtkWidget *compare = FeedNamed(gtk_button_new_with_label("Reconcile selected account"),
                                   "bank-feed-reconcile");
  gtk_box_append(GTK_BOX(root), compare);
  g_signal_connect_object(compare, "clicked", G_CALLBACK(FeedStatementClicked), window, 0);
  FeedComparisonControls(ui, root, window);
  return window;
}
static void FeedOpen(GtkButton *button, gpointer unused) {
  (void)unused;
  GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
  if (!GTK_IS_WINDOW(root) || !UmiGtk4WindowRegistered(GTK_WINDOW(root)))
    return;
  GtkWindow *window = UmiBankFeedGtk4Create(GTK_WINDOW(root));
  if (window != NULL)
    gtk_window_present(window);
}
GtkWidget *UmiBankFeedGtk4LauncherCreate(void) {
  GtkWidget *button =
      FeedNamed(gtk_button_new_with_label("Bank transaction reports"), "bank-feed-launch");
  g_signal_connect(button, "clicked", G_CALLBACK(FeedOpen), NULL);
  return button;
}

bool UmiBankFeedGtk4Busy(GtkWindow *window) {
  if (window == NULL)
    return false;
  const BankFeedUi *ui = g_object_get_data(G_OBJECT(window), BANK_FEED_UI);
  return ui != NULL && ui->loading;
}
