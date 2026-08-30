/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/account_hold.c
 *
 * PURPOSE:
 *   Implement represent ring-fenced account funds and explicit release state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/account_hold.h"
#include <string.h>
UmiStatus umi_banking_account_hold_init(UmiBankingAccountHold *value,
    const char *id,
    const char *account_id,
    int64_t amount_minor,
    bool active) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->account_id,account_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->amount_minor=amount_minor;
    value->active=active;
    return umi_banking_account_hold_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_account_hold_valid(const UmiBankingAccountHold *value) {
    return value!=NULL && (value->amount_minor>0);
}

bool umi_banking_account_hold_releasable(const UmiBankingAccountHold *value) {
    if(value==NULL) return (bool)0;
    return value->active;
}
