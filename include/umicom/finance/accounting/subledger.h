/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/subledger.h
 *
 * PURPOSE:
 *   Represent sub-ledger control-account totals for reconciliation with the general ledger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_SUBLEDGER_H
#define UMICOM_FINANCE_ACCOUNTING_SUBLEDGER_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the accounting subledger data shared with callers of this public contract.
 */
typedef struct UmiAccountingSubledger {
    UmiFinancialId id;
    UmiFinancialId control_account_id;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingSubledger;
/**
 * Initialise accounting subledger from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_accounting_subledger_init(UmiAccountingSubledger *value,
    const char *id,
    const char *control_account_id,
    int64_t debit_minor,
    int64_t credit_minor);
/**
 * Check that accounting subledger satisfies its contract before another service relies on
 * it.
 */
bool umi_accounting_subledger_valid(const UmiAccountingSubledger *value);
/**
 * Provide the accounting subledger net minor operation used by this module and its client
 * applications.
 */
int64_t umi_accounting_subledger_net_minor(const UmiAccountingSubledger *value);
/* Checked net preserves out_minor on refusal; output must not overlap input.
 * Invalid identifiers or negative debit/credit totals return INVALID_ARGUMENT.
 * The legacy scalar getter returns zero on refusal. Initialization preserves
 * the complete destination on failure, including aliased identifier input. */
UmiStatus umi_accounting_subledger_net_minor_checked(const UmiAccountingSubledger *value,
    int64_t *out_minor);
#ifdef __cplusplus
}
#endif
#endif
