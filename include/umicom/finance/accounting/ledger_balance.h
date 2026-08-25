/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/ledger_balance.h
 *
 * PURPOSE:
 *   Calculate ledger-account net balance from debit/credit totals and normal side.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_LEDGER_BALANCE_H
#define UMICOM_FINANCE_ACCOUNTING_LEDGER_BALANCE_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingLedgerBalance {
    UmiFinancialId id;
    UmiFinancialId account_id;
    UmiAccountingNormalSide normal_side;
    int64_t debit_minor;
    int64_t credit_minor;
} UmiAccountingLedgerBalance;
UmiStatus umi_accounting_ledger_balance_init(UmiAccountingLedgerBalance *value,
    const char *id,
    const char *account_id,
    UmiAccountingNormalSide normal_side,
    int64_t debit_minor,
    int64_t credit_minor);
bool umi_accounting_ledger_balance_valid(const UmiAccountingLedgerBalance *value);
int64_t umi_accounting_ledger_balance_net_minor(const UmiAccountingLedgerBalance *value);
#ifdef __cplusplus
}
#endif
#endif
