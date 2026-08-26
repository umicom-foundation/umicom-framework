/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/integrated_stress_result.c
 *
 * PURPOSE:
 *   Combine capital and liquidity stress outcomes into one control decision.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/integrated_stress_result.h"

UmiStatus umi_pru_integrated_stress_result_evaluate(UmiPrudentialIntegratedStressResult *result,int capital_passed,int liquidity_passed) { if(result==NULL||(capital_passed!=0&&capital_passed!=1)||(liquidity_passed!=0&&liquidity_passed!=1))return UMI_STATUS_INVALID_ARGUMENT; result->capital_passed=capital_passed; result->liquidity_passed=liquidity_passed; result->overall_passed=(capital_passed&&liquidity_passed)?1:0; result->severity=result->overall_passed?UMI_PRU_SEVERITY_INFO:UMI_PRU_SEVERITY_BREACH; return UMI_STATUS_OK; }
