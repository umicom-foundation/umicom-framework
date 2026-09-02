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
/**
 * Represent the banking interest rate data shared with callers of this public contract.
 */
typedef struct UmiBankingInterestRate {
    UmiFinancialId id;
    int32_t annual_rate_bps;
    uint32_t day_count_basis;
} UmiBankingInterestRate;
/**
 * Initialise banking interest rate from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_banking_interest_rate_init(UmiBankingInterestRate *value,
    const char *id,
    int32_t annual_rate_bps,
    uint32_t day_count_basis);
/**
 * Check that banking interest rate satisfies its contract before another service relies on
 * it.
 */
bool umi_banking_interest_rate_valid(const UmiBankingInterestRate *value);
/**
 * Provide the banking interest rate rate bps operation used by this module and its client
 * applications.
 */
int32_t umi_banking_interest_rate_rate_bps(const UmiBankingInterestRate *value);
#ifdef __cplusplus
}
#endif
#endif
