/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_account.c
 *
 * PURPOSE:
 *   Implement represent general-ledger accounts, normal side and posting eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_account.h"
#include <string.h>
/*
 * Initialise accounting ledger account from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id,
    const char *name,
    UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side,
    bool posting_allowed) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK)return rc;
    value->account_class=account_class;
    value->normal_side=normal_side;
    value->posting_allowed=posting_allowed;
    return umi_accounting_ledger_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that accounting ledger account satisfies its contract before another service
 * relies on it.
 */
bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value) {
    return value!=NULL && (value->name[0]!='\0' && value->account_class>=UMI_ACCOUNTING_ASSET && value->account_class<=UMI_ACCOUNTING_EXPENSE && (value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT||value->normal_side==UMI_ACCOUNTING_NORMAL_CREDIT));
}

/*
 * Provide the accounting ledger account postable operation used by this module and its
 * client applications.
 */
bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (bool)0;
    return value->posting_allowed;
}
