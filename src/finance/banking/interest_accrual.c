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
#include <limits.h>
#include <stdint.h>
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
/* The value-only entry point now delegates to checked arithmetic to avoid signed overflow and division by zero. The previous implementation is retained for engineering review. */
#if 0
int64_t umi_banking_interest_accrual_accrued_minor(const UmiBankingInterestAccrual *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return (int64_t)0;
    return (value->principal_minor*(int64_t)value->annual_rate_bps*(int64_t)value->days)/((int64_t)10000*(int64_t)value->day_count_basis);
}
#endif
/* Checked arithmetic belongs in the portable finance contract so every
 * banking frontend shares the same rounding and overflow rules. Decomposing
 * both factors around the denominator bounds the remainder product by d*d. */
UmiStatus umi_banking_interest_accrual_calculate(
    const UmiBankingInterestAccrual *value, int64_t *out_minor)
{
    uint64_t principal, numerator, denominator, whole, part, remainder, limit;
    bool negative;
    uintptr_t source, output;
    if (value == NULL || out_minor == NULL || !umi_banking_interest_accrual_valid(value))
        return UMI_STATUS_INVALID_ARGUMENT;
    source = (uintptr_t)value; output = (uintptr_t)out_minor;
    if ((output >= source && output - source < sizeof *value) ||
        (source > output && source - output < sizeof *out_minor))
        return UMI_STATUS_INVALID_ARGUMENT;
    negative = value->annual_rate_bps < 0;
    principal = (uint64_t)value->principal_minor;
    numerator = (uint64_t)(negative ? -(int64_t)value->annual_rate_bps :
        (int64_t)value->annual_rate_bps) * (uint64_t)value->days;
    denominator = UINT64_C(10000) * (uint64_t)value->day_count_basis;
    limit = (uint64_t)INT64_MAX + (negative ? UINT64_C(1) : UINT64_C(0));
    whole = numerator / denominator;
    remainder = numerator % denominator;
    if (whole != 0U && principal > limit / whole) return UMI_STATUS_CAPACITY_EXCEEDED;
    whole *= principal;
    part = principal / denominator;
    if (remainder != 0U && part > (limit - whole) / remainder)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    whole += part * remainder;
    /* Both remainders are below 3,650,000, so this product fits uint64_t. */
    part = ((principal % denominator) * remainder) / denominator;
    if (part > limit - whole) return UMI_STATUS_CAPACITY_EXCEEDED;
    whole += part;
    *out_minor = negative ? (whole == (uint64_t)INT64_MAX + UINT64_C(1) ?
        INT64_MIN : -(int64_t)whole) : (int64_t)whole;
    return UMI_STATUS_OK;
}

int64_t umi_banking_interest_accrual_accrued_minor(const UmiBankingInterestAccrual *value)
{
    int64_t result = 0;
    return umi_banking_interest_accrual_calculate(value, &result) == UMI_STATUS_OK ? result : 0;
}
