/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_batch_report.c
 *
 * PURPOSE:
 *   Qualify exact captured journal reports, text boundaries and independent ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_batch_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
typedef struct Fixture {
  UmiAccountingJournalEntry entries[UMI_ACCOUNTING_MAX_ITEMS];
  UmiAccountingChartOfAccounts chart;
  UmiAccountingAccountingCalendar calendar;
  UmiAccountingJournalBook book;
} Fixture;
static void Entry(UmiAccountingJournalEntry *entry, const char *id, int64_t amount) {
  CHECK(umi_accounting_journal_entry_init(entry, id, (UmiFinancialDate){2026, 10U, 26U}) ==
        UMI_STATUS_OK);
  UmiAccountingJournalLine line;
  CHECK(umi_accounting_journal_line_init(&line, "debit", "cash", amount, 0) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_line_init(&line, "credit", "income", 0, amount) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
}
static void Setup(Fixture *f, size_t count, int64_t amount) {
  UmiAccountingLedgerAccount account;
  CHECK(umi_accounting_ledger_account_init(&account, "cash", "Cash", UMI_ACCOUNTING_ASSET,
                                           UMI_ACCOUNTING_NORMAL_DEBIT, true) == UMI_STATUS_OK);
  CHECK(umi_accounting_chart_of_accounts_add(&f->chart, &account) == UMI_STATUS_OK);
  CHECK(umi_accounting_ledger_account_init(&account, "income", "Income", UMI_ACCOUNTING_REVENUE,
                                           UMI_ACCOUNTING_NORMAL_CREDIT, true) == UMI_STATUS_OK);
  CHECK(umi_accounting_chart_of_accounts_add(&f->chart, &account) == UMI_STATUS_OK);
  UmiAccountingAccountingPeriod period;
  CHECK(umi_accounting_accounting_period_init(&period, "october", (UmiFinancialDate){2026, 10U, 1U},
                                              (UmiFinancialDate){2026, 10U, 31U},
                                              UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
  CHECK(umi_accounting_accounting_calendar_add(&f->calendar, &period) == UMI_STATUS_OK);
  for (size_t i = 0U; i < count; ++i) {
    char id[32];
    int n = snprintf(id, sizeof id, "journal-%zu", i + 1U);
    CHECK(n > 0 && (size_t)n < sizeof id);
    Entry(&f->entries[i], id, amount);
  }
}
static size_t Occurrences(const char *text, const char *needle) {
  size_t count = 0U;
  for (const char *p = text; (p = strstr(p, needle)) != NULL; p += strlen(needle))
    ++count;
  return count;
}
int main(int argc, char **argv) {
  static const char *const names[] = {"basic",
                                      "scale-zero",
                                      "scale-nine",
                                      "maximum-amount",
                                      "wide-integer",
                                      "small-amount",
                                      "order",
                                      "formula-text",
                                      "quoted-text",
                                      "unicode",
                                      "invalid-utf8",
                                      "control-text",
                                      "null-review",
                                      "null-currency",
                                      "lowercase-currency",
                                      "short-currency",
                                      "unterminated-currency",
                                      "invalid-scale",
                                      "null-output",
                                      "zero-budget",
                                      "over-budget",
                                      "short-budget",
                                      "exact-budget",
                                      "late-budget",
                                      "source-edit",
                                      "destroy-review",
                                      "after-apply",
                                      "independent",
                                      "maximum-batch",
                                      "maximum-lines",
                                      "currency-copy"};
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  bool known = false;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(mode, names[i]))
      known = true;
  if (!known)
    return 2;
  size_t count = !strcmp(mode, "maximum-batch") || !strcmp(mode, "maximum-lines")
                     ? UMI_ACCOUNTING_MAX_ITEMS
                     : 2U;
  int64_t amount = !strcmp(mode, "maximum-amount") ? INT64_MAX
                   : !strcmp(mode, "wide-integer") ? INT64_C(9007199254740993)
                   : !strcmp(mode, "small-amount") ? 1
                                                   : 1250;
  Fixture *f = calloc(1U, sizeof *f);
  CHECK(f != NULL);
  Setup(f, count, amount);
  if (!strcmp(mode, "maximum-lines")) {
    for (size_t i = 0U; i < count; ++i) {
      f->entries[i].line_count = 0U;
      for (size_t j = 0U; j < UMI_ACCOUNTING_MAX_LINES; ++j) {
        char id[32];
        int n = snprintf(id, sizeof id, "line-%zu", j + 1U);
        CHECK(n > 0 && (size_t)n < sizeof id);
        UmiAccountingJournalLine line;
        CHECK(umi_accounting_journal_line_init(&line, id, j % 2U ? "income" : "cash",
                                               j % 2U ? 0 : 1, j % 2U ? 1 : 0) == UMI_STATUS_OK);
        CHECK(umi_accounting_journal_entry_add_line(&f->entries[i], &line) == UMI_STATUS_OK);
      }
    }
  }
  if (!strcmp(mode, "formula-text"))
    strcpy(f->entries[1].id.value, "=SUM(1,2)");
  if (!strcmp(mode, "quoted-text"))
    strcpy(f->entries[1].id.value, "receipt,\"quoted\"\r\nline");
  if (!strcmp(mode, "unicode"))
    strcpy(f->entries[1].id.value, "caf\xc3\xa9");
  if (!strcmp(mode, "invalid-utf8")) {
    f->entries[1].id.value[0] = (char)0xff;
    f->entries[1].id.value[1] = '\0';
  }
  if (!strcmp(mode, "control-text")) {
    f->entries[1].lines[1].id.value[0] = '\1';
    f->entries[1].lines[1].id.value[1] = '\0';
  }
  UmiAccountingPostingBatchReview *review = NULL;
  CHECK(umi_accounting_posting_batch_create(f->entries, count, &f->chart, &f->calendar, &f->book,
                                            NULL, &review) == UMI_STATUS_OK);
  UmiCurrency currency = {{'G', 'B', 'P', '\0'}};
  uint8_t scale = !strcmp(mode, "scale-zero") ? 0U : !strcmp(mode, "scale-nine") ? 9U : 2U;
  if (!strcmp(mode, "lowercase-currency"))
    currency.code[0] = 'g';
  if (!strcmp(mode, "short-currency"))
    currency.code[2] = '\0';
  if (!strcmp(mode, "unterminated-currency"))
    currency.code[3] = 'X';
  if (!strcmp(mode, "invalid-scale"))
    scale = 10U;
  size_t limit = !strcmp(mode, "zero-budget")    ? 0U
                 : !strcmp(mode, "over-budget")  ? UMI_CSV_MAX_BYTES + 1U
                 : !strcmp(mode, "short-budget") ? 1U
                                                 : UMI_CSV_MAX_BYTES;
  UmiCsvDocument *report = NULL;
  UmiStatus status =
      umi_accounting_posting_batch_report(!strcmp(mode, "null-review") ? NULL : review,
                                          !strcmp(mode, "null-currency") ? NULL : &currency, scale,
                                          limit, !strcmp(mode, "null-output") ? NULL : &report);
  if (!strcmp(mode, "invalid-utf8") || !strcmp(mode, "control-text") ||
      !strcmp(mode, "null-review") || !strcmp(mode, "null-currency") ||
      !strcmp(mode, "lowercase-currency") || !strcmp(mode, "short-currency") ||
      !strcmp(mode, "unterminated-currency") || !strcmp(mode, "invalid-scale") ||
      !strcmp(mode, "null-output") || !strcmp(mode, "zero-budget") ||
      !strcmp(mode, "over-budget") || !strcmp(mode, "short-budget")) {
    CHECK(status != UMI_STATUS_OK && report == NULL);
    if (!strcmp(mode, "short-budget"))
      CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
  } else {
    CHECK(status == UMI_STATUS_OK && report != NULL && f->book.count == 0U);
    size_t lines = !strcmp(mode, "maximum-lines") ? UMI_ACCOUNTING_MAX_LINES : 2U;
    CHECK(UmiCsvDocumentRows(report) == 1U + count * (lines + 1U));
    const char *text = UmiCsvDocumentData(report);
    CHECK(Occurrences(text, "\"journal-total\"") == count);
    CHECK(Occurrences(text, "\"captured-eligibility\"") == count * (lines + 1U));
    CHECK(strstr(text, "\"2026-10-26\",\"october\"") != NULL);
    CHECK(strstr(text, "posted") == NULL && strstr(text, "settled") == NULL);
    if (!strcmp(mode, "basic"))
      CHECK(strstr(text, "\"1250\",\"0\",\"12.50\",\"0.00\"") != NULL);
    if (!strcmp(mode, "scale-zero"))
      CHECK(strstr(text, "\"1250\",\"0\",\"1250\",\"0\"") != NULL);
    if (!strcmp(mode, "scale-nine"))
      CHECK(strstr(text, "\"0.000001250\"") != NULL);
    if (!strcmp(mode, "small-amount"))
      CHECK(strstr(text, "\"1\",\"0\",\"0.01\",\"0.00\"") != NULL);
    if (!strcmp(mode, "maximum-amount"))
      CHECK(strstr(text, "\"9223372036854775807\",\"0\",\"92233720368547758.07\"") != NULL);
    if (!strcmp(mode, "wide-integer"))
      CHECK(strstr(text, "\"9007199254740993\",\"0\",\"90071992547409.93\"") != NULL);
    if (!strcmp(mode, "formula-text"))
      CHECK(strstr(text, "\"'=SUM(1,2)\"") != NULL);
    if (!strcmp(mode, "quoted-text"))
      CHECK(strstr(text, "\"receipt,\"\"quoted\"\"\r\nline\"") != NULL);
    if (!strcmp(mode, "unicode"))
      CHECK(strstr(text, "caf\xc3\xa9") != NULL);
    if (!strcmp(mode, "order")) {
      const char *a = strstr(text, "\"line\",\"captured-eligibility\",\"1\"");
      const char *b = strstr(text, "\"journal-total\",\"captured-eligibility\",\"1\"");
      const char *c = strstr(text, "\"line\",\"captured-eligibility\",\"2\"");
      CHECK(a && b && c && a < b && b < c);
    }
    if (!strcmp(mode, "exact-budget") || !strcmp(mode, "late-budget")) {
      size_t bytes = UmiCsvDocumentBytes(report);
      UmiCsvDocument *second = NULL;
      size_t cap = !strcmp(mode, "exact-budget") ? bytes + 1U : bytes / 2U;
      status = umi_accounting_posting_batch_report(review, &currency, scale, cap, &second);
      if (!strcmp(mode, "exact-budget")) {
        CHECK(status == UMI_STATUS_OK && second && strcmp(text, UmiCsvDocumentData(second)) == 0);
        UmiCsvDocumentDestroy(second);
        second = NULL;
        CHECK(umi_accounting_posting_batch_report(review, &currency, scale, bytes, &second) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              second == NULL);
      } else
        CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED && second == NULL);
    }
    if (!strcmp(mode, "source-edit") || !strcmp(mode, "after-apply") ||
        !strcmp(mode, "independent")) {
      if (!strcmp(mode, "source-edit")) {
        f->entries[1].lines[0].debit_minor = 999;
        f->chart.items[0].posting_allowed = false;
        f->calendar.items[0].status = UMI_ACCOUNTING_PERIOD_CLOSED;
      } else if (!strcmp(mode, "after-apply")) {
        CHECK(umi_accounting_posting_batch_apply(review, f->entries, count, &f->chart, &f->calendar,
                                                 &f->book) == UMI_STATUS_OK);
        CHECK(f->book.count == count);
      }
      UmiCsvDocument *second = NULL;
      CHECK(umi_accounting_posting_batch_report(review, &currency, scale, UMI_CSV_MAX_BYTES,
                                                &second) == UMI_STATUS_OK);
      CHECK(second != report && strcmp(text, UmiCsvDocumentData(second)) == 0);
      UmiCsvDocumentDestroy(second);
    }
    if (!strcmp(mode, "currency-copy")) {
      currency.code[0] = 'U';
      currency.code[1] = 'S';
      currency.code[2] = 'D';
      CHECK(strstr(text, "\"GBP\",\"2\"") != NULL && strstr(text, "USD") == NULL);
    }
    if (!strcmp(mode, "destroy-review")) {
      umi_accounting_posting_batch_destroy(review);
      review = NULL;
      CHECK(strstr(UmiCsvDocumentData(report), "\"journal-2\"") != NULL);
    }
  }
  UmiCsvDocumentDestroy(report);
  umi_accounting_posting_batch_destroy(review);
  free(f);
  return 0;
}
