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
/**
 * Represent the treasury liquidity profile data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryLiquidityProfile { UmiTreasuryLiquidityBucket buckets[UMI_TREASURY_MAX_BUCKETS]; size_t count; } UmiTreasuryLiquidityProfile;
/**
 * Initialise treasury liquidity profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_liquidity_profile_init(UmiTreasuryLiquidityProfile *profile);
/**
 * Add treasury liquidity profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_liquidity_profile_add(UmiTreasuryLiquidityProfile *profile,const UmiTreasuryLiquidityBucket *bucket);
/**
 * Provide the treasury liquidity profile cumulative gap minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_liquidity_profile_cumulative_gap_minor(const UmiTreasuryLiquidityProfile *profile);
#endif
