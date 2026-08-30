/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/capital_stress_result.c
 *
 * PURPOSE:
 *   Represent post-stress capital ratio and regulatory headroom.
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

#include "umicom/finance/prudential/capital_stress_result.h"

UmiStatus umi_pru_capital_stress_result_evaluate(UmiPrudentialCapitalStressResult *result,double stressed_ratio,double minimum_ratio) { if(result==NULL||!umi_pru_number_valid(stressed_ratio)||!umi_pru_number_valid(minimum_ratio)||minimum_ratio<0.0)return UMI_STATUS_INVALID_ARGUMENT; result->stressed_ratio=stressed_ratio; result->minimum_ratio=minimum_ratio; result->headroom=stressed_ratio-minimum_ratio; result->passed=result->headroom>=0.0?1:0; return UMI_STATUS_OK; }
