/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_reverse_stress_test.c
 *
 * PURPOSE:
 *   Verify estimate the linear shock required to exhaust available headroom.
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

#include "umicom/finance/prudential/reverse_stress_test.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialReverseStressTest r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_reverse_stress_test_calculate(&r,20.0,100.0,0.5)!=UMI_STATUS_OK)return 1; return r.required_shock==0.4?0:2; }
