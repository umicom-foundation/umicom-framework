/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_stress_factor.c
 *
 * PURPOSE:
 *   Verify define a named multiplicative stress factor with direction and severity.
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

#include "umicom/finance/prudential/stress_factor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialStressFactor f; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_stress_factor_init(&f,"DEPOSITS",UMI_PRU_STRESS_DECREASE,0.2,UMI_PRU_SEVERITY_BREACH)!=UMI_STATUS_OK)return 1; return f.magnitude==0.2?0:2; }
