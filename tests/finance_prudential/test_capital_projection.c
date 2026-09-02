/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_projection.c
 *
 * PURPOSE:
 *   Verify project capital after earnings, distributions, issuance and deductions.
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

#include "umicom/finance/prudential/capital_projection.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalProjection r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_projection_calculate(&r,100.0,10.0,5.0,3.0,2.0)!=UMI_STATUS_OK)return 1; return r.closing_capital==106.0?0:2; }
