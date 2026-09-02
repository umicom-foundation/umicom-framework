/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_plan.h
 *
 * PURPOSE:
 *   Represent a funded amount and enforce that allocations do not exceed requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#define UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury funding plan data shared with callers of this public contract.
 */
typedef struct UmiTreasuryFundingPlan {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t requirement_minor;
    int64_t allocated_minor;
} UmiTreasuryFundingPlan;
/**
 * Initialise treasury funding plan from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor);
/**
 * Check that treasury funding plan satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value);
/**
 * Provide the treasury funding plan remaining minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value);
#ifdef __cplusplus
}
#endif
#endif
