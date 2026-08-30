/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_liquidity_profile.c
 *
 * PURPOSE:
 *   Verify ordered liquidity bucket aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_profile.h"
int main(void){UmiTreasuryLiquidityProfile p;UmiTreasuryLiquidityBucket b;umi_treasury_liquidity_profile_init(&p);if(umi_treasury_liquidity_bucket_init(&b,"0-7",0,7,100,140)!=UMI_STATUS_OK)return 1;if(umi_treasury_liquidity_profile_add(&p,&b)!=UMI_STATUS_OK)return 2;if(umi_treasury_liquidity_profile_cumulative_gap_minor(&p)!=-40)return 3;return 0;}
