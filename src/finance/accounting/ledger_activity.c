/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_activity.c
 *
 * PURPOSE:
 *   Derive exact journal activity with whole-input validation and immutable result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_activity.h"
#include "umicom/finance/accounting/ledger_balance.h"
#include <stdlib.h>

struct UmiAccountingLedgerActivity {
  UmiAccountingLedgerActivitySummary summary;
  UmiAccountingLedgerActivityRow rows[UMI_ACCOUNTING_MAX_ITEMS];
};
static size_t ActivityAccount(const UmiAccountingChartOfAccounts *chart, const UmiFinancialId *id) {
  for (size_t i = 0U; i < chart->count; ++i)
    if (umi_financial_id_compare(&chart->items[i].id, id) == 0)
      return i;
  return SIZE_MAX;
}
static UmiStatus ActivityAdd(int64_t current, int64_t incoming, int64_t *out) {
  /* Journal validation establishes nonnegative coefficients. Keep this helper
   * checked independently so later callers cannot reintroduce signed overflow. */
  if (current < 0 || incoming < 0)
    return UMI_STATUS_INVALID_STATE;
  if (incoming > INT64_MAX - current)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  *out = current + incoming;
  return UMI_STATUS_OK;
}
UmiStatus umi_accounting_ledger_activity_create(const UmiAccountingJournalBook *book,
                                                const UmiAccountingChartOfAccounts *chart,
                                                UmiFinancialDate first_date,
                                                UmiFinancialDate last_date,
                                                UmiAccountingLedgerActivity **out_activity) {
  if (out_activity == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out_activity = NULL;
  if (book == NULL || chart == NULL || !umi_financial_date_is_valid(first_date) ||
      !umi_financial_date_is_valid(last_date) ||
      umi_financial_date_compare(first_date, last_date) > 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_accounting_chart_of_accounts_validate(chart);
  if (status != UMI_STATUS_OK)
    return status;
  status = umi_accounting_journal_book_validate(book);
  if (status != UMI_STATUS_OK)
    return status;
  UmiAccountingLedgerActivity *activity = calloc(1U, sizeof *activity);
  if (activity == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  activity->summary.first_date = first_date;
  activity->summary.last_date = last_date;
  activity->summary.scanned_journals = book->count;
  activity->summary.account_count = chart->count;
  for (size_t i = 0U; i < chart->count; ++i)
    activity->rows[i].account = chart->items[i];
  for (size_t i = 0U; status == UMI_STATUS_OK && i < book->count; ++i) {
    const UmiAccountingJournalEntry *entry = &book->items[i];
    bool selected = umi_financial_date_compare(entry->accounting_date, first_date) >= 0 &&
                    umi_financial_date_compare(entry->accounting_date, last_date) <= 0;
    if (selected)
      ++activity->summary.selected_journals;
    for (size_t j = 0U; status == UMI_STATUS_OK && j < entry->line_count; ++j) {
      const UmiAccountingJournalLine *line = &entry->lines[j];
      size_t index = ActivityAccount(chart, &line->account_id);
      if (index == SIZE_MAX) {
        status = UMI_STATUS_NOT_FOUND;
        break;
      }
      if (!selected)
        continue;
      UmiAccountingLedgerActivityRow *row = &activity->rows[index];
      status = ActivityAdd(row->debit_minor, line->debit_minor, &row->debit_minor);
      if (status == UMI_STATUS_OK)
        status = ActivityAdd(row->credit_minor, line->credit_minor, &row->credit_minor);
      if (status == UMI_STATUS_OK)
        status = ActivityAdd(activity->summary.debit_minor, line->debit_minor,
                             &activity->summary.debit_minor);
      if (status == UMI_STATUS_OK)
        status = ActivityAdd(activity->summary.credit_minor, line->credit_minor,
                             &activity->summary.credit_minor);
      if (status == UMI_STATUS_OK) {
        ++row->line_count;
        ++activity->summary.line_count;
      }
    }
  }
  for (size_t i = 0U; status == UMI_STATUS_OK && i < chart->count; ++i) {
    UmiAccountingLedgerActivityRow *row = &activity->rows[i];
    UmiAccountingLedgerBalance balance;
    /* Reuse the existing normal-side contract instead of duplicating sign
     * conventions in Bank, Accountant, TMS or their report widgets. */
    status = umi_accounting_ledger_balance_init(&balance, row->account.id.value,
                                                row->account.id.value, row->account.normal_side,
                                                row->debit_minor, row->credit_minor);
    if (status == UMI_STATUS_OK)
      status = umi_accounting_ledger_balance_net_minor_checked(&balance, &row->normal_net_minor);
    if (row->line_count != 0U)
      ++activity->summary.active_accounts;
  }
  if (status == UMI_STATUS_OK && activity->summary.debit_minor != activity->summary.credit_minor)
    status = UMI_STATUS_INVALID_STATE;
  if (status != UMI_STATUS_OK) {
    free(activity);
    return status;
  }
  *out_activity = activity;
  return UMI_STATUS_OK;
}
void umi_accounting_ledger_activity_destroy(UmiAccountingLedgerActivity *activity) {
  free(activity);
}
const UmiAccountingLedgerActivitySummary *
umi_accounting_ledger_activity_summary(const UmiAccountingLedgerActivity *activity) {
  return activity != NULL ? &activity->summary : NULL;
}
const UmiAccountingLedgerActivityRow *
umi_accounting_ledger_activity_at(const UmiAccountingLedgerActivity *activity, size_t index) {
  return activity != NULL && index < activity->summary.account_count ? &activity->rows[index]
                                                                     : NULL;
}
const UmiAccountingLedgerActivityRow *
umi_accounting_ledger_activity_find(const UmiAccountingLedgerActivity *activity,
                                    const char *account_id) {
  UmiFinancialId id = {0};
  if (activity == NULL || umi_financial_id_assign(&id, account_id) != UMI_STATUS_OK ||
      !umi_financial_id_is_valid(&id))
    return NULL;
  for (size_t i = 0U; i < activity->summary.account_count; ++i)
    if (umi_financial_id_compare(&activity->rows[i].account.id, &id) == 0)
      return &activity->rows[i];
  return NULL;
}
