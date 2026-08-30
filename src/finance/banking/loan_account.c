/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/loan_account.c
 *
 * PURPOSE:
 *   Implement represent lending principal, outstanding balance, rate and lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_account.h"
#include <string.h>
UmiStatus umi_banking_loan_account_init(UmiBankingLoanAccount *value,
    const char *id,
    const char *customer_id,
    const char *product_id,
    int64_t principal_minor,
    int64_t outstanding_minor,
    int32_t annual_rate_bps,
    UmiBankingLoanStatus status) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->customer_id,customer_id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->product_id,product_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->principal_minor=principal_minor;
    value->outstanding_minor=outstanding_minor;
    value->annual_rate_bps=annual_rate_bps;
    value->status=status;
    return umi_banking_loan_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_loan_account_valid(const UmiBankingLoanAccount *value) {
    return value!=NULL && (value->principal_minor>0 && value->outstanding_minor>=0 && value->outstanding_minor<=value->principal_minor && value->status>=UMI_BANKING_LOAN_APPLICATION && value->status<=UMI_BANKING_LOAN_DEFAULTED);
}

int64_t umi_banking_loan_account_repaid_minor(const UmiBankingLoanAccount *value) {
    if(value==NULL) return (int64_t)0;
    return value->principal_minor-value->outstanding_minor;
}
