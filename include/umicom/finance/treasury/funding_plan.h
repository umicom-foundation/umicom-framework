/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/funding_plan.h
 *
 * PURPOSE:
 *   Represent a funded amount and enforce that allocations do not exceed requirement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#define UMICOM_FINANCE_TREASURY_FUNDING_PLAN_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryFundingPlan {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t requirement_minor;
    int64_t allocated_minor;
} UmiTreasuryFundingPlan;
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor);
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value);
int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value);
#ifdef __cplusplus
}
#endif
#endif
