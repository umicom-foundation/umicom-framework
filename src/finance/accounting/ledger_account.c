/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/accounting/ledger_account.c
 *
 * PURPOSE:
 *   Implement represent general-ledger accounts, normal side and posting eligibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/ledger_account.h"
#include <string.h>
UmiStatus umi_accounting_ledger_account_init(UmiAccountingLedgerAccount *value,
    const char *id,
    const char *name,
    UmiAccountingAccountClass account_class,
    UmiAccountingNormalSide normal_side,
    bool posting_allowed) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_accounting_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_financial_core_copy(value->name,sizeof value->name,name);
    if(rc!=UMI_STATUS_OK)return rc;
    value->account_class=account_class;
    value->normal_side=normal_side;
    value->posting_allowed=posting_allowed;
    return umi_accounting_ledger_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_accounting_ledger_account_valid(const UmiAccountingLedgerAccount *value) {
    return value!=NULL && (value->name[0]!='\0' && value->account_class>=UMI_ACCOUNTING_ASSET && value->account_class<=UMI_ACCOUNTING_EXPENSE && (value->normal_side==UMI_ACCOUNTING_NORMAL_DEBIT||value->normal_side==UMI_ACCOUNTING_NORMAL_CREDIT));
}

bool umi_accounting_ledger_account_postable(const UmiAccountingLedgerAccount *value) {
    if(value==NULL) return (bool)0;
    return value->posting_allowed;
}
