/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/liquidity_profile.h
 *
 * PURPOSE:
 *   Maintain ordered liquidity buckets and calculate cumulative liquidity gaps.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_LIQUIDITY_PROFILE_H
#define UMICOM_FINANCE_TREASURY_LIQUIDITY_PROFILE_H
#include "umicom/finance/treasury/liquidity_bucket.h"
typedef struct UmiTreasuryLiquidityProfile { UmiTreasuryLiquidityBucket buckets[UMI_TREASURY_MAX_BUCKETS]; size_t count; } UmiTreasuryLiquidityProfile;
void umi_treasury_liquidity_profile_init(UmiTreasuryLiquidityProfile *profile);
UmiStatus umi_treasury_liquidity_profile_add(UmiTreasuryLiquidityProfile *profile,const UmiTreasuryLiquidityBucket *bucket);
int64_t umi_treasury_liquidity_profile_cumulative_gap_minor(const UmiTreasuryLiquidityProfile *profile);
#endif
