/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/interest_rate.h
 *
 * PURPOSE:
 *   Represent annualised banking interest rates with an explicit day-count basis.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_INTEREST_RATE_H
#define UMICOM_FINANCE_BANKING_INTEREST_RATE_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBankingInterestRate {
    UmiFinancialId id;
    int32_t annual_rate_bps;
    uint32_t day_count_basis;
} UmiBankingInterestRate;
UmiStatus umi_banking_interest_rate_init(UmiBankingInterestRate *value,
    const char *id,
    int32_t annual_rate_bps,
    uint32_t day_count_basis);
bool umi_banking_interest_rate_valid(const UmiBankingInterestRate *value);
int32_t umi_banking_interest_rate_rate_bps(const UmiBankingInterestRate *value);
#ifdef __cplusplus
}
#endif
#endif
