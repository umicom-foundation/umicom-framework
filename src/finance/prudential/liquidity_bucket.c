/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_bucket.c
 *
 * PURPOSE:
 *   Aggregate inflows, outflows and closing liquidity for one horizon bucket.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_bucket.h"

UmiStatus umi_pru_liquidity_bucket_calculate(UmiPrudentialLiquidityBucket *bucket,int32_t day,double opening,double inflows,double outflows) { if(bucket==NULL||day<0||opening<0.0||inflows<0.0||outflows<0.0)return UMI_STATUS_INVALID_ARGUMENT; bucket->horizon_day=day; bucket->opening_liquidity=opening; bucket->inflows=inflows; bucket->outflows=outflows; bucket->closing_liquidity=opening+inflows-outflows; return umi_pru_number_valid(bucket->closing_liquidity)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
