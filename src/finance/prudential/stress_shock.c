/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/prudential/stress_shock.c
 *
 * PURPOSE:
 *   Apply a percentage shock to a base prudential value.
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

#include "umicom/finance/prudential/stress_shock.h"

/*
 * Perform pru stress shock through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_pru_stress_shock_apply(UmiPrudentialStressShock *result,double base_value,double shock_rate) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(result==NULL||!umi_pru_number_valid(base_value)||!umi_pru_number_valid(shock_rate)||shock_rate<-1.0)return UMI_STATUS_INVALID_ARGUMENT; result->base_value=base_value; result->shock_rate=shock_rate; result->stressed_value=base_value*(1.0+shock_rate); return umi_pru_number_valid(result->stressed_value)?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE; }
