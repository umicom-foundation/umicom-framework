/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/loan_installment.c
 *
 * PURPOSE:
 *   Implement represent scheduled lending principal/interest and payment progress.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_installment.h"
#include <string.h>
UmiStatus umi_banking_loan_installment_init(UmiBankingLoanInstallment *value,
    const char *id,
    const char *loan_id,
    UmiFinancialDate due_date,
    int64_t principal_minor,
    int64_t interest_minor,
    int64_t paid_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->loan_id,loan_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->due_date=due_date;
    value->principal_minor=principal_minor;
    value->interest_minor=interest_minor;
    value->paid_minor=paid_minor;
    return umi_banking_loan_installment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_loan_installment_valid(const UmiBankingLoanInstallment *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->loan_id) && umi_financial_date_is_valid(value->due_date) && value->principal_minor>=0 && value->interest_minor>=0 && value->paid_minor>=0 && value->paid_minor<=value->principal_minor+value->interest_minor);
}

int64_t umi_banking_loan_installment_outstanding_minor(const UmiBankingLoanInstallment *value) {
    if(value==NULL) return (int64_t)0;
    return value->principal_minor+value->interest_minor-value->paid_minor;
}
