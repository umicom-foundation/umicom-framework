/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/sensitivity_stress.c
 *
 * PURPOSE:
 *   Calculate stressed value from a sensitivity and market-factor shock.
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

#include "umicom/finance/prudential/sensitivity_stress.h"

/*
 * Provide the pru sensitivity stress calculate operation used by this module and its
 * client applications.
 */
UmiStatus umi_pru_sensitivity_stress_calculate(UmiPrudentialSensitivityStress *result,double base,double sensitivity,double shock) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||!umi_pru_number_valid(base)||!umi_pru_number_valid(sensitivity)||!umi_pru_number_valid(shock))return UMI_STATUS_INVALID_ARGUMENT; result->base_value=base; result->sensitivity=sensitivity; result->factor_shock=shock; result->stressed_value=base+sensitivity*shock; return umi_pru_number_valid(result->stressed_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
