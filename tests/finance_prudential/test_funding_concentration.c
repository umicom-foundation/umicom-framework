/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_funding_concentration.c
 *
 * PURPOSE:
 *   Verify measure largest-source concentration within total funding.
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

#include "umicom/finance/prudential/funding_concentration.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialFundingConcentration r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_funding_concentration_calculate(&r,12.0,100.0)!=UMI_STATUS_OK)return 1; return r.ratio==0.12?0:2; }
