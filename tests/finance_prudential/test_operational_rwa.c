/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_operational_rwa.c
 *
 * PURPOSE:
 *   Verify calculate operational risk-weighted assets from business indicator and loss multiplier.
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

#include "umicom/finance/prudential/operational_rwa.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialOperationalRwa r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_operational_rwa_calculate(&r,8.0,1.0,12.5)!=UMI_STATUS_OK)return 1; return r.rwa==100.0?0:2; }
