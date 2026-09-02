/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_ladder.c
 *
 * PURPOSE:
 *   Build a bounded ordered liquidity ladder and cumulative balance.
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

#include "umicom/finance/prudential/liquidity_ladder.h"

/* Add pru liquidity ladder only after its inputs and available capacity have been checked. */
UmiStatus umi_pru_liquidity_ladder_add(UmiPrudentialLiquidityLadder *ladder,const UmiPrudentialLiquidityBucket *bucket) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(ladder==NULL||bucket==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(ladder->count>=UMI_PRU_MAX_PERIODS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(ladder->count>0U&&bucket->horizon_day<=ladder->buckets[ladder->count-1U].horizon_day)return UMI_STATUS_INVALID_STATE; ladder->buckets[ladder->count++]=*bucket; return UMI_STATUS_OK; }
/*
 * Provide the pru liquidity ladder closing operation used by this module and its client
 * applications.
 */
double umi_pru_liquidity_ladder_closing(const UmiPrudentialLiquidityLadder *ladder) { return ladder==NULL||ladder->count==0U?0.0:ladder->buckets[ladder->count-1U].closing_liquidity; }
