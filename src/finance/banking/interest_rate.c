/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/banking/interest_rate.c
 *
 * PURPOSE:
 *   Implement represent annualised banking interest rates with an explicit day-count basis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/interest_rate.h"
#include <string.h>
UmiStatus umi_banking_interest_rate_init(UmiBankingInterestRate *value,
    const char *id,
    int32_t annual_rate_bps,
    uint32_t day_count_basis) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    UmiStatus rc=umi_banking_id_assign(&value->id,id);
    if(rc!=UMI_STATUS_OK) return rc;
    value->annual_rate_bps=annual_rate_bps;
    value->day_count_basis=day_count_basis;
    return umi_banking_interest_rate_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_banking_interest_rate_valid(const UmiBankingInterestRate *value) {
    return value!=NULL && (value->annual_rate_bps>=-10000 && value->annual_rate_bps<=100000 && (value->day_count_basis==360U||value->day_count_basis==365U));
}

int32_t umi_banking_interest_rate_rate_bps(const UmiBankingInterestRate *value) {
    if(value==NULL) return (int32_t)0;
    return value->annual_rate_bps;
}
