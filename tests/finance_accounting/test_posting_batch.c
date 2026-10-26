/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_batch.c
 *
 * PURPOSE:
 *   Qualify complete journal publication, first-refusal detail and stale review rejection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_batch.h"
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

static void Entry(UmiAccountingJournalEntry *entry, const char *id, int64_t minor) {
  CHECK(umi_accounting_journal_entry_init(entry, id, (UmiFinancialDate){2026, 10U, 26U}) ==
        UMI_STATUS_OK);
  UmiAccountingJournalLine line;
  CHECK(umi_accounting_journal_line_init(&line, "debit", "cash", minor, 0) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_line_init(&line, "credit", "income", 0, minor) == UMI_STATUS_OK);
  CHECK(umi_accounting_journal_entry_add_line(entry, &line) == UMI_STATUS_OK);
}
static void Setup(Fixture *f) {
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
  CHECK(umi_accounting_accounting_period_init(
            &period, "november", (UmiFinancialDate){2026, 11U, 1U},
            (UmiFinancialDate){2026, 11U, 30U}, UMI_ACCOUNTING_PERIOD_OPEN) == UMI_STATUS_OK);
  CHECK(umi_accounting_accounting_calendar_add(&f->calendar, &period) == UMI_STATUS_OK);
  Entry(&f->entries[0], "first", 1250);
  Entry(&f->entries[1], "second", 2700);
}
static void FillBook(Fixture *f, size_t count) {
  UmiAccountingJournalEntry *entry = malloc(sizeof *entry);
  CHECK(entry != NULL);
  for (size_t i = 0U; i < count; ++i) {
    char id[32];
    int n = snprintf(id, sizeof id, "prior-%zu", i);
    CHECK(n > 0 && (size_t)n < sizeof id);
    Entry(entry, id, 100);
    CHECK(umi_accounting_journal_book_post(&f->book, entry) == UMI_STATUS_OK);
  }
  free(entry);
}
static UmiStatus Create(Fixture *f, size_t count, UmiAccountingPostingBatchPreview *preview,
                        UmiAccountingPostingBatchReview **review) {
  return umi_accounting_posting_batch_create(f->entries, count, &f->chart, &f->calendar, &f->book,
                                             preview, review);
}
static UmiStatus Apply(Fixture *f, size_t count, UmiAccountingPostingBatchReview *review) {
  return umi_accounting_posting_batch_apply(review, f->entries, count, &f->chart, &f->calendar,
                                            &f->book);
}
static void Posted(Fixture *f, size_t count) {
  CHECK(f->book.count == count);
  CHECK(umi_accounting_journal_book_validate(&f->book) == UMI_STATUS_OK);
  for (size_t i = 0U; i < count; ++i) {
    CHECK(strcmp(f->book.items[i].id.value, f->entries[i].id.value) == 0);
    CHECK(f->book.items[i].status == UMI_ACCOUNTING_JOURNAL_POSTED);
    CHECK(f->entries[i].status == UMI_ACCOUNTING_JOURNAL_DRAFT);
    CHECK(f->book.items[i].lines[0].debit_minor == f->entries[i].lines[0].debit_minor);
  }
}
int main(int argc, char **argv) {
  static const char *const names[] = {"apply",
                                      "inspect",
                                      "preview",
                                      "empty",
                                      "too-many",
                                      "null-entries",
                                      "null-chart",
                                      "null-calendar",
                                      "null-book",
                                      "null-output",
                                      "null-inspection",
                                      "null-accessors",
                                      "optional-preview",
                                      "second-unbalanced",
                                      "second-overflow",
                                      "second-account",
                                      "second-closed",
                                      "second-posted",
                                      "duplicate-batch",
                                      "duplicate-book",
                                      "invalid-chart",
                                      "invalid-calendar",
                                      "invalid-book",
                                      "one-slot",
                                      "maximum",
                                      "full-book",
                                      "stale-amount",
                                      "stale-id",
                                      "stale-date",
                                      "stale-state",
                                      "stale-order",
                                      "stale-line-count",
                                      "stale-chart",
                                      "stale-chart-count",
                                      "stale-period",
                                      "stale-calendar-count",
                                      "stale-book",
                                      "stale-book-count",
                                      "stale-length",
                                      "different-entries",
                                      "different-chart",
                                      "different-calendar",
                                      "different-book",
                                      "null-review-apply",
                                      "null-entry-apply",
                                      "null-chart-apply",
                                      "null-calendar-apply",
                                      "null-book-apply",
                                      "tails",
                                      "retry-after-stale",
                                      "captured-immutable",
                                      "inspect-preserves",
                                      "replay"};
  if (argc != 2)
    return 2;
  const char *name = argv[1];
  bool known = false;
  for (size_t i = 0U; i < sizeof names / sizeof names[0]; ++i)
    known = known || strcmp(name, names[i]) == 0;
  if (!known)
    return 2;
  Fixture *f = calloc(1U, sizeof *f), *saved = calloc(1U, sizeof *saved),
          *other = calloc(1U, sizeof *other);
  CHECK(f && saved && other);
  Setup(f);
  *other = *f;
  UmiAccountingPostingBatchReview *review = NULL;
  UmiAccountingPostingBatchPreview preview = {0}, sentinel;
  memset(&sentinel, 0x5a, sizeof sentinel);
  size_t count = 2U;
  if (!strcmp(name, "null-accessors")) {
    CHECK(umi_accounting_posting_batch_preview(NULL) == NULL);
    CHECK(umi_accounting_posting_batch_entry(NULL, 0U) == NULL);
    CHECK(umi_accounting_posting_batch_entry_preview(NULL, 0U) == NULL);
    umi_accounting_posting_batch_destroy(NULL);
    goto done;
  }
  if (!strcmp(name, "empty") || !strcmp(name, "too-many") || !strcmp(name, "null-entries") ||
      !strcmp(name, "null-chart") || !strcmp(name, "null-calendar") || !strcmp(name, "null-book") ||
      !strcmp(name, "null-output") || !strcmp(name, "null-inspection")) {
    preview = sentinel;
    *saved = *f;
    if (!strcmp(name, "null-inspection")) {
      CHECK(umi_accounting_posting_batch_inspect(f->entries, 2U, &f->chart, &f->calendar, &f->book,
                                                 NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else {
      count = !strcmp(name, "empty") ? 0U : !strcmp(name, "too-many") ? SIZE_MAX : 2U;
      review = (UmiAccountingPostingBatchReview *)(void *)f;
      CHECK(umi_accounting_posting_batch_create(
                !strcmp(name, "null-entries") ? NULL : f->entries, count,
                !strcmp(name, "null-chart") ? NULL : &f->chart,
                !strcmp(name, "null-calendar") ? NULL : &f->calendar,
                !strcmp(name, "null-book") ? NULL : &f->book, &preview,
                !strcmp(name, "null-output") ? NULL : &review) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(!strcmp(name, "null-output") || review == NULL);
      review = NULL;
    }
    CHECK(memcmp(&preview, &sentinel, sizeof preview) == 0);
    CHECK(memcmp(f, saved, sizeof *f) == 0);
    goto done;
  }
  bool refusal = false;
  UmiAccountingPostingBlocker blocker = UMI_ACCOUNTING_POSTING_READY;
  size_t refused = 1U;
  if (!strcmp(name, "second-unbalanced")) {
    f->entries[1].lines[1].credit_minor = 1;
    blocker = UMI_ACCOUNTING_POSTING_UNBALANCED;
    refusal = true;
  } else if (!strcmp(name, "second-overflow")) {
    f->entries[1].lines[0].debit_minor = INT64_MAX;
    f->entries[1].lines[2] = f->entries[1].lines[0];
    CHECK(umi_accounting_id_assign(&f->entries[1].lines[2].id, "extra") == UMI_STATUS_OK);
    f->entries[1].line_count = 3U;
    blocker = UMI_ACCOUNTING_POSTING_AMOUNT_OVERFLOW;
    refusal = true;
  } else if (!strcmp(name, "second-account")) {
    CHECK(umi_accounting_id_assign(&f->entries[1].lines[1].account_id, "unknown") == UMI_STATUS_OK);
    blocker = UMI_ACCOUNTING_POSTING_MISSING_ACCOUNT;
    refusal = true;
  } else if (!strcmp(name, "second-closed")) {
    f->entries[1].accounting_date.month = 11U;
    f->calendar.items[1].status = UMI_ACCOUNTING_PERIOD_CLOSED;
    blocker = UMI_ACCOUNTING_POSTING_CLOSED_PERIOD;
    refusal = true;
  } else if (!strcmp(name, "second-posted")) {
    f->entries[1].status = UMI_ACCOUNTING_JOURNAL_POSTED;
    blocker = UMI_ACCOUNTING_POSTING_ENTRY_STATE;
    refusal = true;
  } else if (!strcmp(name, "duplicate-batch")) {
    f->entries[1].id = f->entries[0].id;
    blocker = UMI_ACCOUNTING_POSTING_DUPLICATE_JOURNAL;
    refusal = true;
  } else if (!strcmp(name, "duplicate-book")) {
    CHECK(umi_accounting_journal_book_post(&f->book, &f->entries[1]) == UMI_STATUS_OK);
    blocker = UMI_ACCOUNTING_POSTING_DUPLICATE_JOURNAL;
    refusal = true;
  } else if (!strcmp(name, "invalid-chart")) {
    f->chart.count = SIZE_MAX;
    blocker = UMI_ACCOUNTING_POSTING_INVALID_CHART;
    refusal = true;
    refused = 0U;
  } else if (!strcmp(name, "invalid-calendar")) {
    f->calendar.count = SIZE_MAX;
    blocker = UMI_ACCOUNTING_POSTING_INVALID_CALENDAR;
    refusal = true;
    refused = 0U;
  } else if (!strcmp(name, "invalid-book")) {
    f->book.count = SIZE_MAX;
    blocker = UMI_ACCOUNTING_POSTING_INVALID_BOOK;
    refusal = true;
    refused = 0U;
  } else if (!strcmp(name, "one-slot") || !strcmp(name, "full-book")) {
    FillBook(f,
             !strcmp(name, "one-slot") ? UMI_ACCOUNTING_MAX_ITEMS - 1U : UMI_ACCOUNTING_MAX_ITEMS);
    blocker = UMI_ACCOUNTING_POSTING_FULL_BOOK;
    refusal = true;
    refused = !strcmp(name, "one-slot") ? 1U : 0U;
  }
  if (refusal) {
    *saved = *f;
    CHECK(umi_accounting_posting_batch_inspect(f->entries, count, &f->chart, &f->calendar, &f->book,
                                               &preview) == UMI_STATUS_OK);
    CHECK(preview.failed_index == refused && preview.ready_count == refused);
    CHECK(preview.detail.blocker == blocker && preview.refusal != UMI_STATUS_OK);
    CHECK(preview.line_count == refused * 2U);
    CHECK(memcmp(f, saved, sizeof *f) == 0);
    UmiStatus create_status = Create(f, count, &preview, &review);
    CHECK(create_status == preview.refusal);
    CHECK(review == NULL && memcmp(f, saved, sizeof *f) == 0);
    goto done;
  }
  if (!strcmp(name, "maximum")) {
    count = UMI_ACCOUNTING_MAX_ITEMS;
    for (size_t i = 0U; i < count; ++i) {
      char id[32];
      int n = snprintf(id, sizeof id, "entry-%zu", i);
      CHECK(n > 0 && (size_t)n < sizeof id);
      Entry(&f->entries[i], id, 100);
    }
  }
  if (!strcmp(name, "stale-book") || !strcmp(name, "stale-book-count"))
    FillBook(f, 1U);
  *saved = *f;
  CHECK(Create(f, count, !strcmp(name, "optional-preview") ? NULL : &preview, &review) ==
        UMI_STATUS_OK);
  CHECK(review != NULL && memcmp(f, saved, sizeof *f) == 0);
  const UmiAccountingPostingBatchPreview *p = umi_accounting_posting_batch_preview(review);
  CHECK(p && p->entry_count == count && p->ready_count == count && p->line_count == count * 2U);
  CHECK(p->failed_index == SIZE_MAX && p->refusal == UMI_STATUS_OK);
  CHECK(umi_accounting_posting_batch_entry(review, count) == NULL);
  CHECK(umi_accounting_posting_batch_entry_preview(review, SIZE_MAX) == NULL);
  CHECK(umi_accounting_posting_batch_entry_preview(review, 0U)->refusal == UMI_STATUS_OK);
  if (!strcmp(name, "inspect") || !strcmp(name, "inspect-preserves") || !strcmp(name, "preview")) {
    CHECK(umi_accounting_posting_batch_inspect(f->entries, count, &f->chart, &f->calendar, &f->book,
                                               &preview) == UMI_STATUS_OK);
    CHECK(preview.entry_count == count && preview.ready_count == count);
    CHECK(memcmp(f, saved, sizeof *f) == 0);
    goto done;
  }
  bool stale = strncmp(name, "stale-", 6U) == 0 || !strcmp(name, "retry-after-stale") ||
               !strcmp(name, "captured-immutable");
  if (!strcmp(name, "stale-amount") || !strcmp(name, "retry-after-stale") ||
      !strcmp(name, "captured-immutable")) {
    ++f->entries[1].lines[0].debit_minor;
    ++f->entries[1].lines[1].credit_minor;
  } else if (!strcmp(name, "stale-id"))
    CHECK(umi_accounting_id_assign(&f->entries[1].id, "changed") == UMI_STATUS_OK);
  else if (!strcmp(name, "stale-date"))
    ++f->entries[1].accounting_date.day;
  else if (!strcmp(name, "stale-state"))
    f->entries[1].status = UMI_ACCOUNTING_JOURNAL_VALIDATED;
  else if (!strcmp(name, "stale-order")) {
    UmiAccountingJournalEntry *tmp = malloc(sizeof *tmp);
    CHECK(tmp);
    *tmp = f->entries[0];
    f->entries[0] = f->entries[1];
    f->entries[1] = *tmp;
    free(tmp);
  } else if (!strcmp(name, "stale-line-count"))
    f->entries[1].line_count = SIZE_MAX;
  else if (!strcmp(name, "stale-chart"))
    f->chart.items[1].posting_allowed = false;
  else if (!strcmp(name, "stale-chart-count"))
    f->chart.count = SIZE_MAX;
  else if (!strcmp(name, "stale-period"))
    f->calendar.items[1].status = UMI_ACCOUNTING_PERIOD_CLOSED;
  else if (!strcmp(name, "stale-calendar-count"))
    f->calendar.count = SIZE_MAX;
  else if (!strcmp(name, "stale-book")) {
    ++f->book.items[0].lines[0].debit_minor;
    ++f->book.items[0].lines[1].credit_minor;
  } else if (!strcmp(name, "stale-book-count"))
    f->book.count = SIZE_MAX;
  else if (!strcmp(name, "stale-length"))
    count = 1U;
  if (stale) {
    UmiAccountingJournalBook *before = malloc(sizeof *before);
    CHECK(before);
    *before = f->book;
    CHECK(Apply(f, count, review) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(before, &f->book, sizeof *before) == 0);
    free(before);
    CHECK(umi_accounting_posting_batch_entry(review, 1U)->lines[0].debit_minor == 2700);
    if (!strcmp(name, "retry-after-stale")) {
      *f = *saved;
      CHECK(Apply(f, 2U, review) == UMI_STATUS_OK);
      Posted(f, 2U);
    }
    goto done;
  }
  if (strncmp(name, "different-", 10U) == 0 || strstr(name, "-apply") != NULL) {
    bool nulls = strncmp(name, "null-", 5U) == 0;
    *saved = *f;
    CHECK(umi_accounting_posting_batch_apply(!strcmp(name, "null-review-apply") ? NULL : review,
                                             !strcmp(name, "null-entry-apply")    ? NULL
                                             : !strcmp(name, "different-entries") ? other->entries
                                                                                  : f->entries,
                                             count,
                                             !strcmp(name, "null-chart-apply")  ? NULL
                                             : !strcmp(name, "different-chart") ? &other->chart
                                                                                : &f->chart,
                                             !strcmp(name, "null-calendar-apply") ? NULL
                                             : !strcmp(name, "different-calendar")
                                                 ? &other->calendar
                                                 : &f->calendar,
                                             !strcmp(name, "null-book-apply")  ? NULL
                                             : !strcmp(name, "different-book") ? &other->book
                                                                               : &f->book) ==
          (nulls ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_INVALID_STATE));
    CHECK(memcmp(f, saved, sizeof *f) == 0 && other->book.count == 0U);
    goto done;
  }
  if (!strcmp(name, "tails")) {
    f->entries[0].id.value[strlen(f->entries[0].id.value) + 1U] = 'x';
    memset(&f->entries[0].lines[2], 0xa5, sizeof f->entries[0].lines[2]);
    memset(&f->entries[2], 0xa5, sizeof f->entries[2]);
    f->chart.items[0].name[strlen(f->chart.items[0].name) + 1U] = 'x';
    memset(&f->chart.items[2], 0xa5, sizeof f->chart.items[2]);
    memset(&f->calendar.items[2], 0xa5, sizeof f->calendar.items[2]);
    memset(&f->book.items[0], 0xa5, sizeof f->book.items[0]);
  }
  CHECK(Apply(f, count, review) == UMI_STATUS_OK);
  Posted(f, count);
  *saved = *f;
  CHECK(Apply(f, count, review) == UMI_STATUS_INVALID_STATE);
  CHECK(memcmp(f, saved, sizeof *f) == 0);
done:
  umi_accounting_posting_batch_destroy(review);
  free(other);
  free(saved);
  free(f);
  return 0;
}
