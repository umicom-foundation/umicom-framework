/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/banking/interest_accrual.h
 *
 * PURPOSE:
 *   Calculate deterministic simple-interest accrual in minor units for banking balances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_BANKING_INTEREST_ACCRUAL_H
#define UMICOM_FINANCE_BANKING_INTEREST_ACCRUAL_H
#include "umicom/finance/banking/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the banking interest accrual data shared with callers of this public contract.
 */
typedef struct UmiBankingInterestAccrual {
    UmiFinancialId id;
    int64_t principal_minor;
    int32_t annual_rate_bps;
    uint32_t days;
    uint32_t day_count_basis;
} UmiBankingInterestAccrual;
/**
 * Initialise banking interest accrual from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_banking_interest_accrual_init(UmiBankingInterestAccrual *value,
    const char *id,
    int64_t principal_minor,
    int32_t annual_rate_bps,
    uint32_t days,
    uint32_t day_count_basis);
/**
 * Check that banking interest accrual satisfies its contract before another service relies
 * on it.
 */
bool umi_banking_interest_accrual_valid(const UmiBankingInterestAccrual *value);
/**
 * Provide the banking interest accrual accrued minor operation used by this module and its
 * client applications.
 */
int64_t umi_banking_interest_accrual_accrued_minor(const UmiBankingInterestAccrual *value);
/** Checked simple interest, truncated toward zero once after the full rational
 * calculation. No floating point or overflowing intermediate product is used.
 * Supports signed rates, nonnegative principal, 0..3660 days and a 360/365 basis.
 * Returns CAPACITY_EXCEEDED when the final amount cannot fit int64_t; invalid
 * inputs return INVALID_ARGUMENT. The separate output is unchanged on failure.
 * Prefer this API to the historical value-only function, which returns zero
 * on error and therefore cannot distinguish failure from zero interest. */
UmiStatus umi_banking_interest_accrual_calculate(
    const UmiBankingInterestAccrual *value, int64_t *out_minor);

#ifdef __cplusplus
}
#endif
#endif
