/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/banking_snapshot.c
 *
 * PURPOSE:
 *   Implement capture aggregate customer, deposit and lending counts and balances for operational views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_snapshot.h"
#include <string.h>
UmiStatus umi_banking_banking_snapshot_init(UmiBankingBankingSnapshot *value,
    const char *id,
    size_t customer_count,
    size_t deposit_account_count,
    size_t loan_count,
    int64_t deposit_balance_minor,
    int64_t loan_balance_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->customer_count=customer_count;
    value->deposit_account_count=deposit_account_count;
    value->loan_count=loan_count;
    value->deposit_balance_minor=deposit_balance_minor;
    value->loan_balance_minor=loan_balance_minor;
    return umi_banking_banking_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_banking_snapshot_valid(const UmiBankingBankingSnapshot *value) {
    return value!=NULL && (value->deposit_balance_minor>=0 && value->loan_balance_minor>=0);
}

int64_t umi_banking_banking_snapshot_net_funding_minor(const UmiBankingBankingSnapshot *value) {
    if(value==NULL) return (int64_t)0;
    return value->deposit_balance_minor-value->loan_balance_minor;
}
