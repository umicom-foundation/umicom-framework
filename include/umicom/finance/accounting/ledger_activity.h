/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/ledger_activity.h
 *
 * PURPOSE:
 *   Capture bounded exact account activity from a complete posted journal book.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_LEDGER_ACTIVITY_H
#define UMICOM_FINANCE_ACCOUNTING_LEDGER_ACTIVITY_H
#include "umicom/finance/accounting/chart_of_accounts.h"
#include "umicom/finance/accounting/journal_book.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingLedgerActivitySummary {
  UmiFinancialDate first_date;
  UmiFinancialDate last_date;
  size_t scanned_journals;
  size_t selected_journals;
  size_t account_count;
  size_t active_accounts;
  size_t line_count;
  int64_t debit_minor;
  int64_t credit_minor;
} UmiAccountingLedgerActivitySummary;
typedef struct UmiAccountingLedgerActivityRow {
  UmiAccountingLedgerAccount account;
  size_t line_count;
  int64_t debit_minor;
  int64_t credit_minor;
  int64_t normal_net_minor;
} UmiAccountingLedgerActivityRow;
typedef struct UmiAccountingLedgerActivity UmiAccountingLedgerActivity;

/* Capture whole-journal movements within inclusive first_date..last_date.
 * Rows follow chart order and include accounts with zero selected activity.
 * Normal net is debit-credit for debit-normal accounts, credit-debit otherwise.
 * An account that currently disallows posting still retains its historical rows.
 *
 * Complete book and chart validation runs before traversal. Every stored line,
 * including outside the requested dates, must reference this chart; missing
 * accounts return NOT_FOUND. Only selected movements are summed. Per-account
 * and overall nonnegative debit/credit totals must fit int64_t; overflow returns
 * CAPACITY_EXCEEDED even when a net result might fit. Nothing is saturated.
 *
 * This is activity within the chosen range, not an opening/closing balance:
 * external carry-forward amounts and journals outside the range are not added.
 * These containers have no currency metadata. The owner must establish one
 * consistent currency/minor-unit convention before calling. No conversion,
 * reconciliation, authorization or external completeness assertion is made.
 *
 * Call on a serialized owner; inputs are borrowed only during this call.
 * Output storage must not overlap inputs. *out_activity is NULL on any failure;
 * successful output owns all rows independently of the input lifetime. */
UmiStatus umi_accounting_ledger_activity_create(const UmiAccountingJournalBook *book,
                                                const UmiAccountingChartOfAccounts *chart,
                                                UmiFinancialDate first_date,
                                                UmiFinancialDate last_date,
                                                UmiAccountingLedgerActivity **out_activity);
void umi_accounting_ledger_activity_destroy(UmiAccountingLedgerActivity *activity);
/* Borrow immutable captured data until destroy. NULL or out-of-range returns
 * NULL. Find uses exact account identity; unknown or malformed IDs return NULL. */
const UmiAccountingLedgerActivitySummary *
umi_accounting_ledger_activity_summary(const UmiAccountingLedgerActivity *activity);
const UmiAccountingLedgerActivityRow *
umi_accounting_ledger_activity_at(const UmiAccountingLedgerActivity *activity, size_t index);
const UmiAccountingLedgerActivityRow *
umi_accounting_ledger_activity_find(const UmiAccountingLedgerActivity *activity,
                                    const char *account_id);
#ifdef __cplusplus
}
#endif
#endif
