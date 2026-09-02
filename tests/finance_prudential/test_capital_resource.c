/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_resource.c
 *
 * PURPOSE:
 *   Verify represent an eligible capital resource with adjustments and deductions.
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

#include "umicom/finance/prudential/capital_resource.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalResource r; UmiCurrency c={"GBP"}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_resource_init(&r,"RES",UMI_PRU_CAPITAL_CET1,120.0,20.0,c)!=UMI_STATUS_OK)return 1; return r.eligible_amount==100.0?0:2; }
