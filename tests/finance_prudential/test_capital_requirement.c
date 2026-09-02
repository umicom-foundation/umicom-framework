/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_requirement.c
 *
 * PURPOSE:
 *   Verify aggregate minimum capital and additional buffer requirements.
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

#include "umicom/finance/prudential/capital_requirement.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalRequirement r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_requirement_calculate(&r,80.0,25.0,10.0,5.0)!=UMI_STATUS_OK)return 1; return r.total_requirement==120.0?0:2; }
