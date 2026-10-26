/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/ledger_balance.h
 *
 * PURPOSE:
 *   Calculate ledger-account net balance from debit/credit totals and normal side.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_LEDGER_BALANCE_H
#define UMICOM_FINANCE_ACCOUNTING_LEDGER_BALANCE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting ledger balance data shared with callers of this public
 * contract.
 */
typedef struct UmiAccountingLedgerBalance {
    UmiFinancialId id;
    UmiFinancialId account_id;
    UmiAccountingNormalSide normal_side;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingLedgerBalance;
/**
 * Initialise accounting ledger balance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_ledger_balance_init(UmiAccountingLedgerBalance *value,
    const char *id,
    const char *account_id,
    UmiAccountingNormalSide normal_side,
    int64_t debit_minor,
    int64_t credit_minor);
/**
 * Check that accounting ledger balance satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_ledger_balance_valid(const UmiAccountingLedgerBalance *value);
/**
 * Provide the accounting ledger balance net minor operation used by this module and its
 * client applications.
 */
int64_t umi_accounting_ledger_balance_net_minor(const UmiAccountingLedgerBalance *value);
/* Checked net amount validates complete identifiers, side and nonnegative totals.
 * Refusal leaves the distinct, non-overlapping output unchanged. The legacy
 * accessor returns zero on invalid input. Initialization preserves value on failure. */
UmiStatus umi_accounting_ledger_balance_net_minor_checked(
    const UmiAccountingLedgerBalance *value, int64_t *out_minor);
#ifdef __cplusplus
}
#endif
#endif
