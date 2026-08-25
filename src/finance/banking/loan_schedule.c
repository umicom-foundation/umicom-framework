/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/loan_schedule.c
 *
 * PURPOSE:
 *   Implement summarise amortising loan schedule principal, interest and installment count.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_schedule.h"
#include <string.h>
UmiStatus umi_banking_loan_schedule_init(UmiBankingLoanSchedule *value,
    const char *id,
    const char *loan_id,
    uint32_t installment_count,
    int64_t principal_minor,
    int64_t interest_minor) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->loan_id,loan_id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->installment_count=installment_count;
    value->principal_minor=principal_minor;
    value->interest_minor=interest_minor;
    return umi_banking_loan_schedule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_loan_schedule_valid(const UmiBankingLoanSchedule *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->loan_id) && value->installment_count>0U && value->principal_minor>0 && value->interest_minor>=0);
}

int64_t umi_banking_loan_schedule_total_due_minor(const UmiBankingLoanSchedule *value) {
    if(value==NULL) return (int64_t)0;
    return value->principal_minor+value->interest_minor;
}
