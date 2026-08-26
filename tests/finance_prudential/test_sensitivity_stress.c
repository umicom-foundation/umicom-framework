/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_sensitivity_stress.c
 *
 * PURPOSE:
 *   Verify calculate stressed value from a sensitivity and market-factor shock.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/sensitivity_stress.h"

int main(void) { UmiPrudentialSensitivityStress r; if(umi_pru_sensitivity_stress_calculate(&r,100.0,-20.0,0.5)!=UMI_STATUS_OK)return 1; return r.stressed_value==90.0?0:2; }
