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
typedef struct UmiAccountingSubledger {
    UmiFinancialId id;
    UmiFinancialId control_account_id;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingSubledger;
UmiStatus umi_accounting_subledger_init(UmiAccountingSubledger *value,
    const char *id,
    const char *control_account_id,
    int64_t debit_minor,
    int64_t credit_minor);
bool umi_accounting_subledger_valid(const UmiAccountingSubledger *value);
int64_t umi_accounting_subledger_net_minor(const UmiAccountingSubledger *value);
#ifdef __cplusplus
}
#endif
#endif
