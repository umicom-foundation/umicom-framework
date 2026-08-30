/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/accounting/ledger_account.h
 *
 * PURPOSE:
 *   Represent general-ledger accounts, normal side and posting eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_ACCOUNTING_LEDGER_ACCOUNT_H
#define UMICOM_FINANCE_ACCOUNTING_LEDGER_ACCOUNT_H
#include "umicom/finance/accounting/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAccountingLedgerAccount {
    UmiFinancialId id;
    char name[UMI_ACCOUNTING_NAME_CAPACITY];
    UmiAccountingAccountClass account_class;
    UmiAccountingNormalSide normal_side;
    bool posting_allowed;
} UmiAccountingLedgerAccount;
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id,
    const char *name,
    UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side,
    bool posting_allowed);
bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value);
bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value);
#ifdef __cplusplus
}
#endif
#endif
