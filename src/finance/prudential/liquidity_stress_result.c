/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/liquidity_stress_result.c
 *
 * PURPOSE:
 *   Represent post-stress liquidity ratio and regulatory headroom.
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

#include "umicom/finance/prudential/liquidity_stress_result.h"

/*
 * Provide the pru liquidity stress result evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_pru_liquidity_stress_result_evaluate(UmiPrudentialLiquidityStressResult *result,double stressed_ratio,double minimum_ratio) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||!umi_pru_number_valid(stressed_ratio)||!umi_pru_number_valid(minimum_ratio)||minimum_ratio<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->stressed_ratio=stressed_ratio; result->minimum_ratio=minimum_ratio; result->headroom=stressed_ratio-minimum_ratio; result->passed=result->headroom>=0.0?1:0; return UMI_STATUS_OK; }
