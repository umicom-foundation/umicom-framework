/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/journal_batch/main.c
 *
 * PURPOSE:
 *   Demonstrate reviewed all-or-nothing publication into a local journal book.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_activity.h"
#include "umicom/finance/accounting/posting_batch.h"
#include "umicom/finance/accounting/posting_batch_report.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x)                                                                                 \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "Refused at line %d: %s\n", __LINE__, #x);                                   \
      goto done;                                                                                   \
    }                                                                                              \
  } while (0)

static UmiStatus MakeEntry(UmiAccountingJournalEntry *entry, const char *id, int64_t amount) {
  UmiStatus status =
      umi_accounting_journal_entry_init(entry, id, (UmiFinancialDate){2026, 10U, 26U});
  UmiAccountingJournalLine line;
  if (status == UMI_STATUS_OK)
    status = umi_accounting_journal_line_init(&line, "debit", "cash", amount, 0);
  if (status == UMI_STATUS_OK)
    status = umi_accounting_journal_entry_add_line(entry, &line);
  if (status == UMI_STATUS_OK)
    status = umi_accounting_journal_line_init(&line, "credit", "income", 0, amount);
  if (status == UMI_STATUS_OK)
    status = umi_accounting_journal_entry_add_line(entry, &line);
  return status;
}
int main(int argc, char **argv) {
  /* Preserve the original command set while extending the same SDK example
   * with read-only reports. Existing posting scenarios keep their behavior. */
#if 0
  if (argc != 2 || (strcmp(argv[1], "apply") && strcmp(argv[1], "draft-change") &&
                    strcmp(argv[1], "chart-change") && strcmp(argv[1], "duplicate") &&
                    strcmp(argv[1], "later-refusal"))) {
    fputs("Usage: journal-batch apply|draft-change|chart-change|duplicate|later-refusal\n", stderr);
    return 2;
  }
#endif
  if (argc != 2 || (strcmp(argv[1], "apply") && strcmp(argv[1], "draft-change") &&
                    strcmp(argv[1], "chart-change") && strcmp(argv[1], "duplicate") &&
                    strcmp(argv[1], "later-refusal") && strcmp(argv[1], "report") &&
                    strcmp(argv[1], "report-stale") && strcmp(argv[1], "report-after-apply") &&
                    strcmp(argv[1], "activity") && strcmp(argv[1], "activity-range"))) {
    fputs("Usage: journal-batch "
          "apply|draft-change|chart-change|duplicate|later-refusal|report|report-stale|report-"
          "after-apply|activity|activity-range\n",
          stderr);
    return 2;
  }
  int result = 1;
  UmiAccountingPostingBatchReview *review = NULL;
  UmiCsvDocument *report = NULL;
  UmiAccountingLedgerActivity *activity = NULL;
  UmiAccountingJournalEntry *entries = calloc(2U, sizeof *entries);
  UmiAccountingJournalBook *book = calloc(1U, sizeof *book);
  UmiAccountingChartOfAccounts chart = {0};
  UmiAccountingAccountingCalendar calendar = {0};
  REQUIRE(entries && book);
  UmiAccountingLedgerAccount account;
  REQUIRE(umi_accounting_ledger_account_init(&account, "cash", "Practice cash",
                                             UMI_ACCOUNTING_ASSET, UMI_ACCOUNTING_NORMAL_DEBIT,
                                             true) == UMI_STATUS_OK);
  REQUIRE(umi_accounting_chart_of_accounts_add(&chart, &account) == UMI_STATUS_OK);
  REQUIRE(umi_accounting_ledger_account_init(&account, "income", "Practice income",
                                             UMI_ACCOUNTING_REVENUE, UMI_ACCOUNTING_NORMAL_CREDIT,
                                             true) == UMI_STATUS_OK);
  REQUIRE(umi_accounting_chart_of_accounts_add(&chart, &account) == UMI_STATUS_OK);
  UmiAccountingAccountingPeriod period;
  REQUIRE(umi_accounting_accounting_period_init(
              &period, "practice-period", (UmiFinancialDate){2026, 10U, 1U},
              (UmiFinancialDate){2026, 10U, 31U}, UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
  REQUIRE(umi_accounting_accounting_calendar_add(&calendar, &period) == UMI_STATUS_OK);
  /* The example explicitly uses GBP pennies for both accounts. The journal
   * container does not infer currency or convert floating-point amounts. */
  REQUIRE(MakeEntry(&entries[0], "receipt-one", 1250) == UMI_STATUS_OK);
  REQUIRE(MakeEntry(&entries[1], "receipt-two", 750) == UMI_STATUS_OK);
  if (!strcmp(argv[1], "duplicate"))
    entries[1].id = entries[0].id;
  if (!strcmp(argv[1], "later-refusal"))
    entries[1].lines[1].credit_minor = 749;
  UmiAccountingPostingBatchPreview preview;
  UmiStatus status =
      umi_accounting_posting_batch_create(entries, 2U, &chart, &calendar, book, &preview, &review);
  if (!strcmp(argv[1], "duplicate") || !strcmp(argv[1], "later-refusal")) {
    REQUIRE(status != UMI_STATUS_OK && review == NULL && preview.failed_index == 1U &&
            preview.ready_count == 1U);
    REQUIRE(book->count == 0U);
    printf("Entry %zu refused; no journals were posted.\n", preview.failed_index + 1U);
  } else {
    REQUIRE(status == UMI_STATUS_OK && review != NULL && preview.ready_count == 2U &&
            book->count == 0U);
    /* Activity is derived from the posted book, using one inclusive date
     * range. It is independent of any later mutation to the example owners. */
    if (!strcmp(argv[1], "activity") || !strcmp(argv[1], "activity-range")) {
      REQUIRE(umi_accounting_posting_batch_apply(review, entries, 2U, &chart, &calendar, book) ==
              UMI_STATUS_OK);
      UmiFinancialDate first = {2026, 10U, 1U};
      if (!strcmp(argv[1], "activity-range"))
        first.day = 27U;
      REQUIRE(umi_accounting_ledger_activity_create(book, &chart, first,
                                                    (UmiFinancialDate){2026, 10U, 31U},
                                                    &activity) == UMI_STATUS_OK);
      const UmiAccountingLedgerActivitySummary *summary =
          umi_accounting_ledger_activity_summary(activity);
      int64_t expected = !strcmp(argv[1], "activity-range") ? 0 : 2000;
      REQUIRE(summary != NULL && summary->debit_minor == expected &&
              summary->credit_minor == expected);
      REQUIRE(summary->selected_journals == (expected == 0 ? 0U : 2U));
      for (size_t i = 0U; i < summary->account_count; ++i) {
        const UmiAccountingLedgerActivityRow *row = umi_accounting_ledger_activity_at(activity, i);
        REQUIRE(row != NULL && row->normal_net_minor == expected);
        printf("%s: %" PRId64 " GBP minor units of normal-side activity\n", row->account.id.value,
               row->normal_net_minor);
      }
      result = 0;
      goto done;
    }
    /* Reporting consumes captured values only. Later draft edits and local
     * publication cannot change the meaning of an earlier eligibility review. */
    if (!strncmp(argv[1], "report", 6U)) {
      if (!strcmp(argv[1], "report-stale")) {
        entries[0].lines[0].debit_minor = 999;
        entries[0].lines[1].credit_minor = 999;
        REQUIRE(umi_accounting_posting_batch_apply(review, entries, 2U, &chart, &calendar, book) ==
                UMI_STATUS_INVALID_STATE);
      }
      if (!strcmp(argv[1], "report-after-apply")) {
        REQUIRE(umi_accounting_posting_batch_apply(review, entries, 2U, &chart, &calendar, book) ==
                UMI_STATUS_OK);
        REQUIRE(book->count == 2U);
      }
      UmiCurrency currency = {{'G', 'B', 'P', '\0'}};
      REQUIRE(umi_accounting_posting_batch_report(review, &currency, 2U, UMI_CSV_MAX_BYTES,
                                                  &report) == UMI_STATUS_OK);
      REQUIRE(UmiCsvDocumentRows(report) == 7U);
      REQUIRE(strstr(UmiCsvDocumentData(report), "\"12.50\"") != NULL);
      REQUIRE(strstr(UmiCsvDocumentData(report), "captured-eligibility") != NULL);
      printf("%s", UmiCsvDocumentData(report));
      result = 0;
      goto done;
    }
    /* A UI displays these captured values before offering Apply. Review
     * does not grant authenticated posting authority or persist anything. */
    for (size_t i = 0U; i < preview.entry_count; ++i)
      printf("Review journal: %s\n", umi_accounting_posting_batch_entry(review, i)->id.value);
    if (!strcmp(argv[1], "draft-change")) {
      entries[1].lines[0].debit_minor = 751;
      entries[1].lines[1].credit_minor = 751;
    }
    if (!strcmp(argv[1], "chart-change"))
      chart.items[0].posting_allowed = false;
    status = umi_accounting_posting_batch_apply(review, entries, 2U, &chart, &calendar, book);
    if (strcmp(argv[1], "apply")) {
      REQUIRE(status == UMI_STATUS_INVALID_STATE && book->count == 0U);
      puts("Review is stale; no journals were posted.");
    } else {
      REQUIRE(status == UMI_STATUS_OK && book->count == 2U);
      REQUIRE(umi_accounting_journal_book_validate(book) == UMI_STATUS_OK);
      REQUIRE(entries[0].status == UMI_ACCOUNTING_JOURNAL_DRAFT);
      REQUIRE(book->items[0].status == UMI_ACCOUNTING_JOURNAL_POSTED);
      REQUIRE(umi_accounting_posting_batch_apply(review, entries, 2U, &chart, &calendar, book) ==
              UMI_STATUS_INVALID_STATE);
      puts("Two journals published to the local book; repeated Apply refused.");
    }
  }
  result = 0;
done:
  umi_accounting_ledger_activity_destroy(activity);
  UmiCsvDocumentDestroy(report);
  umi_accounting_posting_batch_destroy(review);
  free(book);
  free(entries);
  return result;
}
