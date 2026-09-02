/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/interest_accrual.c
 *
 * PURPOSE:
 *   Implement calculate deterministic simple-interest accrual in minor units for banking balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/interest_accrual.h"
#include <string.h>
/*
 * Initialise banking interest accrual from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_interest_accrual_init(UmiBankingInterestAccrual *value,
    const char *id,
    int64_t principal_minor,
    int32_t annual_rate_bps,
    uint32_t days,
    uint32_t day_count_basis) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(rc!=UMI_STATUS_OK) return rc;
    value->principal_minor=principal_minor;
    value->annual_rate_bps=annual_rate_bps;
    value->days=days;
    value->day_count_basis=day_count_basis;
    return umi_banking_interest_accrual_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that banking interest accrual satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_interest_accrual_valid(const UmiBankingInterestAccrual *value) {
    return value!=NULL && (value->principal_minor>=0 && value->days<=3660U && (value->day_count_basis==360U||value->day_count_basis==365U));
}

/*
 * Provide the banking interest accrual accrued minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_interest_accrual_accrued_minor(const UmiBankingInterestAccrual *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return (value->principal_minor*(int64_t)value->annual_rate_bps*(int64_t)value->days)/((int64_t)10000*(int64_t)value->day_count_basis);
}
