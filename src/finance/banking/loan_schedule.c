/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/loan_schedule.c
 *
 * PURPOSE:
 *   Implement summarise amortising loan schedule principal, interest and installment count.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/loan_schedule.h"
#include <string.h>
/*
 * Initialise banking loan schedule from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_loan_schedule_init(UmiBankingLoanSchedule *value,
    const char *id,
    const char *loan_id,
    uint32_t installment_count,
    int64_t principal_minor,
    int64_t interest_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    rc=umi_banking_id_assign(&value->loan_id,loan_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->installment_count=installment_count;
    value->principal_minor=principal_minor;
    value->interest_minor=interest_minor;
    return umi_banking_loan_schedule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking loan schedule satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_loan_schedule_valid(const UmiBankingLoanSchedule *value) {
    return value!=NULL && (umi_financial_id_is_valid(&value->loan_id) && value->installment_count>0U && value->principal_minor>0 && value->interest_minor>=0);
}

/*
 * Provide the banking loan schedule total due minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_loan_schedule_total_due_minor(const UmiBankingLoanSchedule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return value->principal_minor+value->interest_minor;
}
