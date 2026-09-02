/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_headroom.c
 *
 * PURPOSE:
 *   Verify measure eligible capital headroom over the aggregate requirement.
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

#include "umicom/finance/prudential/capital_headroom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalHeadroom r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_headroom_calculate(&r,120.0,100.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.headroom!=20.0||r.breached!=0)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_headroom_calculate(&r,80.0,100.0)!=UMI_STATUS_OK||r.breached!=1)return 3; return 0; }
