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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/stress_shock.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressShock r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_shock_apply(&r,100.0,-0.2)!=UMI_STATUS_OK)return 1; return r.stressed_value==80.0?0:2; }
