/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_profile.c
 *
 * PURPOSE:
 *   Implement ordered liquidity profile accumulation and gap calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_profile.h"
#include <string.h>
/*
 * Initialise treasury liquidity profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_liquidity_profile_init(UmiTreasuryLiquidityProfile *p){/* Apply this branch only when its contract condition is satisfied. */ if(p) memset(p,0,sizeof *p);}
/*
 * Add treasury liquidity profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_liquidity_profile_add(UmiTreasuryLiquidityProfile *p,const UmiTreasuryLiquidityBucket *b){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p||!umi_treasury_liquidity_bucket_valid(b)) return UMI_STATUS_INVALID_ARGUMENT; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>=UMI_TREASURY_MAX_BUCKETS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(p->count>0U && b->start_days < p->buckets[p->count-1U].start_days)return UMI_STATUS_INVALID_ARGUMENT; p->buckets[p->count++]=*b; return UMI_STATUS_OK;}
/*
 * Provide the treasury liquidity profile cumulative gap minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_liquidity_profile_cumulative_gap_minor(const UmiTreasuryLiquidityProfile *p){int64_t x=0; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!p)return 0; /* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0U;i<p->count;++i)x+=umi_treasury_liquidity_bucket_gap_minor(&p->buckets[i]); return x;}
