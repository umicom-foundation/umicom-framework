/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_profile.c
 *
 * PURPOSE:
 *   Implement ordered liquidity profile accumulation and gap calculation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_profile.h"
#include <string.h>
void umi_treasury_liquidity_profile_init(UmiTreasuryLiquidityProfile *p){if(p) memset(p,0,sizeof *p);}
UmiStatus umi_treasury_liquidity_profile_add(UmiTreasuryLiquidityProfile *p,const UmiTreasuryLiquidityBucket *b){if(!p||!umi_treasury_liquidity_bucket_valid(b)) return UMI_STATUS_INVALID_ARGUMENT; if(p->count>=UMI_TREASURY_MAX_BUCKETS)return UMI_STATUS_CAPACITY_EXCEEDED; if(p->count>0U && b->start_days < p->buckets[p->count-1U].start_days)return UMI_STATUS_INVALID_ARGUMENT; p->buckets[p->count++]=*b; return UMI_STATUS_OK;}
int64_t umi_treasury_liquidity_profile_cumulative_gap_minor(const UmiTreasuryLiquidityProfile *p){int64_t x=0; if(!p)return 0; for(size_t i=0U;i<p->count;++i)x+=umi_treasury_liquidity_bucket_gap_minor(&p->buckets[i]); return x;}
