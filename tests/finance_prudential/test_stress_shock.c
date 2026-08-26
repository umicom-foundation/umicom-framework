/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_shock.c
 *
 * PURPOSE:
 *   Verify apply a percentage shock to a base prudential value.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_shock.h"

int main(void) { UmiPrudentialStressShock r; if(umi_pru_stress_shock_apply(&r,100.0,-0.2)!=UMI_STATUS_OK)return 1; return r.stressed_value==80.0?0:2; }
