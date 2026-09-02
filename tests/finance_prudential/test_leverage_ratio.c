/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_leverage_ratio.c
 *
 * PURPOSE:
 *   Verify calculate the prudential leverage ratio against total exposure measure.
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

#include "umicom/finance/prudential/leverage_ratio.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialLeverageRatio r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_leverage_ratio_calculate(&r,12.0,100.0)!=UMI_STATUS_OK)return 1; return r.ratio==0.12?0:2; }
