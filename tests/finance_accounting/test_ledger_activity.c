/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_ledger_activity.c
 *
 * PURPOSE:
 *   Qualify exact posted-book activity, inclusive dates, ownership and refusal boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_activity.h"
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
  UmiAccountingJournalBook book;
  UmiAccountingChartOfAccounts chart;
} Fixture;
static void Account(Fixture *f, const char *id, UmiAccountingNormalSide side) {
  UmiAccountingLedgerAccount a;
  CHECK(umi_accounting_ledger_account_init(&a, id, id, UMI_ACCOUNTING_ASSET, side, true) ==
        UMI_STATUS_OK);
  CHECK(umi_accounting_chart_of_accounts_add(&f->chart, &a) == UMI_STATUS_OK);
}
static void Post(Fixture *f, const char *id, UmiFinancialDate date, int64_t amount,
                 const char *debit, const char *credit) {
  UmiAccountingJournalEntry *entry = calloc(1U, sizeof *entry);
  CHECK(entry != NULL);
  CHECK(umi_accounting_journal_entry_init(entry, id, date) == UMI_STATUS_OK);
  UmiAccountingJournalLine line;
  CHECK(umi_accounting_journal_line_init(&line, "debit", debit, amount, 0) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_line_init(&line, "credit", credit, 0, amount) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_book_post(&f->book, entry) == UMI_STATUS_OK);
  free(entry);
}
static void Amount(UmiAccountingJournalEntry *entry, int64_t amount) {
  entry->lines[0].debit_minor = amount;
  entry->lines[1].credit_minor = amount;
}
int main(int argc, char **argv) {
  static const char *const names[] = {"basic",
                                      "empty-book",
                                      "empty-chart",
                                      "from-inclusive",
                                      "to-inclusive",
                                      "single-day",
                                      "no-matches",
                                      "normal-sides",
                                      "reversal",
                                      "disabled-account",
                                      "chart-order",
                                      "maximum-amount",
                                      "wide-integer",
                                      "account-overflow",
                                      "total-overflow",
                                      "excluded-overflow",
                                      "missing-account",
                                      "missing-excluded-account",
                                      "invalid-chart-count",
                                      "invalid-book-count",
                                      "duplicate-chart",
                                      "duplicate-book",
                                      "malformed-chart",
                                      "malformed-line",
                                      "draft-in-book",
                                      "unbalanced-book",
                                      "null-book",
                                      "null-chart",
                                      "null-output",
                                      "invalid-first-date",
                                      "invalid-last-date",
                                      "reversed-dates",
                                      "immutable",
                                      "empty-accessors",
                                      "unknown-account",
                                      "maximum-accounts",
                                      "maximum-journals"};
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  bool known = false;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(mode, names[i]))
      known = true;
  if (!known)
    return 2;
  Fixture *f = calloc(1U, sizeof *f);
  CHECK(f != NULL);
  Account(f, "cash", UMI_ACCOUNTING_NORMAL_DEBIT);
  Account(f, "income", UMI_ACCOUNTING_NORMAL_CREDIT);
  Account(f, "unused", UMI_ACCOUNTING_NORMAL_DEBIT);
  Post(f, "first", (UmiFinancialDate){2026, 10U, 1U}, 100, "cash", "income");
  Post(f, "middle", (UmiFinancialDate){2026, 10U, 15U}, 200, "cash", "income");
  Post(f, "future", (UmiFinancialDate){2026, 11U, 1U}, 400, "cash", "income");
  UmiFinancialDate first = {2026, 10U, 1U}, last = {2026, 10U, 31U};
  if (!strcmp(mode, "empty-book"))
    f->book.count = 0U;
  if (!strcmp(mode, "empty-chart")) {
    f->book.count = 0U;
    f->chart.count = 0U;
  }
  if (!strcmp(mode, "from-inclusive"))
    first.day = 15U;
  if (!strcmp(mode, "to-inclusive"))
    last.day = 1U;
  if (!strcmp(mode, "single-day"))
    first.day = last.day = 15U;
  if (!strcmp(mode, "no-matches")) {
    first.month = last.month = 12U;
    last.day = 31U;
  }
  if (!strcmp(mode, "reversal")) {
    CHECK(umi_financial_id_assign(&f->book.items[1].lines[0].account_id, "income") ==
          UMI_STATUS_OK);
    CHECK(umi_financial_id_assign(&f->book.items[1].lines[1].account_id, "cash") == UMI_STATUS_OK);
  }
  if (!strcmp(mode, "normal-sides")) {
    f->chart.items[0].normal_side = UMI_ACCOUNTING_NORMAL_CREDIT;
    f->chart.items[1].normal_side = UMI_ACCOUNTING_NORMAL_DEBIT;
  }
  if (!strcmp(mode, "disabled-account"))
    f->chart.items[0].posting_allowed = false;
  if (!strcmp(mode, "chart-order")) {
    UmiAccountingLedgerAccount a = f->chart.items[0];
    f->chart.items[0] = f->chart.items[2];
    f->chart.items[2] = a;
  }
  if (!strcmp(mode, "maximum-amount") || !strcmp(mode, "wide-integer")) {
    f->book.count = 1U;
    Amount(&f->book.items[0],
           !strcmp(mode, "maximum-amount") ? INT64_MAX : INT64_C(9007199254740993));
  }
  if (!strcmp(mode, "account-overflow") || !strcmp(mode, "total-overflow") ||
      !strcmp(mode, "excluded-overflow")) {
    Amount(&f->book.items[0], INT64_MAX);
    Amount(&f->book.items[1], 1);
    if (!strcmp(mode, "excluded-overflow"))
      first.day = 15U;
    if (!strcmp(mode, "total-overflow")) {
      CHECK(umi_financial_id_assign(&f->book.items[1].lines[0].account_id, "income") ==
            UMI_STATUS_OK);
      CHECK(umi_financial_id_assign(&f->book.items[1].lines[1].account_id, "cash") ==
            UMI_STATUS_OK);
    }
  }
  if (!strcmp(mode, "missing-account") || !strcmp(mode, "missing-excluded-account")) {
    size_t i = !strcmp(mode, "missing-account") ? 0U : 2U;
    CHECK(umi_financial_id_assign(&f->book.items[i].lines[0].account_id, "unregistered") ==
          UMI_STATUS_OK);
  }
  if (!strcmp(mode, "invalid-chart-count"))
    f->chart.count = UMI_ACCOUNTING_MAX_ITEMS + 1U;
  if (!strcmp(mode, "invalid-book-count"))
    f->book.count = UMI_ACCOUNTING_MAX_ITEMS + 1U;
  if (!strcmp(mode, "duplicate-chart"))
    f->chart.items[1].id = f->chart.items[0].id;
  if (!strcmp(mode, "duplicate-book"))
    f->book.items[1].id = f->book.items[0].id;
  if (!strcmp(mode, "malformed-chart"))
    memset(f->chart.items[0].name, 'x', sizeof f->chart.items[0].name);
  if (!strcmp(mode, "malformed-line"))
    f->book.items[1].line_count = UMI_ACCOUNTING_MAX_LINES + 1U;
  if (!strcmp(mode, "draft-in-book"))
    f->book.items[0].status = UMI_ACCOUNTING_JOURNAL_DRAFT;
  if (!strcmp(mode, "unbalanced-book"))
    f->book.items[0].lines[1].credit_minor = 99;
  if (!strcmp(mode, "invalid-first-date"))
    first.day = 0U;
  if (!strcmp(mode, "invalid-last-date"))
    last.month = 13U;
  if (!strcmp(mode, "reversed-dates")) {
    first.day = 20U;
    last.day = 1U;
  }
  if (!strcmp(mode, "maximum-accounts")) {
    for (size_t i = f->chart.count; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
      char id[32];
      int n = snprintf(id, sizeof id, "unused-%zu", i);
      CHECK(n > 0 && (size_t)n < sizeof id);
      Account(f, id, UMI_ACCOUNTING_NORMAL_DEBIT);
    }
  }
  if (!strcmp(mode, "maximum-journals")) {
    f->book.count = 0U;
    for (size_t i = 0U; i < UMI_ACCOUNTING_MAX_ITEMS; ++i) {
      char id[32];
      int n = snprintf(id, sizeof id, "journal-%zu", i);
      CHECK(n > 0 && (size_t)n < sizeof id);
      Post(f, id, (UmiFinancialDate){2026, 10U, 15U}, 1, "cash", "income");
    }
  }
  Fixture *before = malloc(sizeof *before);
  CHECK(before != NULL);
  memcpy(before, f, sizeof *before);
  UmiAccountingLedgerActivity *activity = NULL;
  UmiStatus status = umi_accounting_ledger_activity_create(
      !strcmp(mode, "null-book") ? NULL : &f->book, !strcmp(mode, "null-chart") ? NULL : &f->chart,
      first, last, !strcmp(mode, "null-output") ? NULL : &activity);
  CHECK(memcmp(before, f, sizeof *f) == 0);
  free(before);
  bool bad = !strcmp(mode, "account-overflow") || !strcmp(mode, "total-overflow") ||
             !strcmp(mode, "missing-account") || !strcmp(mode, "missing-excluded-account") ||
             !strcmp(mode, "invalid-chart-count") || !strcmp(mode, "invalid-book-count") ||
             !strcmp(mode, "duplicate-chart") || !strcmp(mode, "duplicate-book") ||
             !strcmp(mode, "malformed-chart") || !strcmp(mode, "malformed-line") ||
             !strcmp(mode, "draft-in-book") || !strcmp(mode, "unbalanced-book") ||
             !strcmp(mode, "null-book") || !strcmp(mode, "null-chart") ||
             !strcmp(mode, "null-output") || !strcmp(mode, "invalid-first-date") ||
             !strcmp(mode, "invalid-last-date") || !strcmp(mode, "reversed-dates");
  if (bad) {
    CHECK(status != UMI_STATUS_OK && activity == NULL);
    if (strstr(mode, "overflow") || strstr(mode, "invalid-chart-count") ||
        strstr(mode, "invalid-book-count"))
      CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    if (strstr(mode, "missing-"))
      CHECK(status == UMI_STATUS_NOT_FOUND);
  } else {
    CHECK(status == UMI_STATUS_OK && activity != NULL);
    const UmiAccountingLedgerActivitySummary *s = umi_accounting_ledger_activity_summary(activity);
    CHECK(s && s->account_count == f->chart.count && s->scanned_journals == f->book.count);
    CHECK(s->debit_minor == s->credit_minor && s->line_count == 2U * s->selected_journals);
    CHECK(umi_accounting_ledger_activity_at(activity, s->account_count) == NULL);
    if (!strcmp(mode, "empty-book") || !strcmp(mode, "empty-chart") ||
        !strcmp(mode, "no-matches")) {
      CHECK(s->debit_minor == 0 && s->selected_journals == 0U && s->active_accounts == 0U);
    } else {
      const UmiAccountingLedgerActivityRow *cash =
          umi_accounting_ledger_activity_find(activity, "cash");
      const UmiAccountingLedgerActivityRow *income =
          umi_accounting_ledger_activity_find(activity, "income");
      CHECK(cash && income && s->active_accounts == 2U);
      if (!strcmp(mode, "normal-sides")) {
        CHECK(cash->normal_net_minor == -300 && income->normal_net_minor == -300);
        CHECK(s->debit_minor == 300 && cash->credit_minor == 0 && income->debit_minor == 0);
      } else if (!strcmp(mode, "reversal")) {
        CHECK(cash->normal_net_minor == -100 && income->normal_net_minor == -100);
        CHECK(cash->debit_minor == 100 && cash->credit_minor == 200);
      } else {
        int64_t expected = !strcmp(mode, "from-inclusive") || !strcmp(mode, "single-day") ? 200
                           : !strcmp(mode, "to-inclusive")                                ? 100
                           : !strcmp(mode, "maximum-amount")    ? INT64_MAX
                           : !strcmp(mode, "wide-integer")      ? INT64_C(9007199254740993)
                           : !strcmp(mode, "excluded-overflow") ? 1
                           : !strcmp(mode, "maximum-journals")  ? (int64_t)UMI_ACCOUNTING_MAX_ITEMS
                                                                : 300;
        CHECK(s->debit_minor == expected && cash->normal_net_minor == expected &&
              income->normal_net_minor == expected);
        CHECK(cash->credit_minor == 0 && income->debit_minor == 0);
      }
      if (!strcmp(mode, "disabled-account"))
        CHECK(!cash->account.posting_allowed);
      if (!strcmp(mode, "chart-order"))
        CHECK(!strcmp(umi_accounting_ledger_activity_at(activity, 0U)->account.id.value, "unused"));
      if (!strcmp(mode, "immutable")) {
        f->chart.items[0].name[0] = 'X';
        Amount(&f->book.items[0], 999);
        free(f);
        f = NULL;
        CHECK(!strcmp(cash->account.name, "cash") && cash->normal_net_minor == 300 &&
              s->debit_minor == 300);
      }
    }
    if (!strcmp(mode, "unknown-account")) {
      CHECK(umi_accounting_ledger_activity_find(activity, "CASH") == NULL);
      CHECK(umi_accounting_ledger_activity_find(activity, "missing") == NULL);
      CHECK(umi_accounting_ledger_activity_find(activity, "") == NULL);
      CHECK(umi_accounting_ledger_activity_find(activity, NULL) == NULL);
    }
    if (!strcmp(mode, "empty-accessors")) {
      CHECK(umi_accounting_ledger_activity_summary(NULL) == NULL);
      CHECK(umi_accounting_ledger_activity_at(NULL, 0U) == NULL);
      CHECK(umi_accounting_ledger_activity_find(NULL, "cash") == NULL);
      umi_accounting_ledger_activity_destroy(NULL);
    }
  }
  umi_accounting_ledger_activity_destroy(activity);
  free(f);
  return 0;
}
