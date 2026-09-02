/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_capital_resource_book.c
 *
 * PURPOSE:
 *   Verify aggregate eligible capital resources and compute tier totals.
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

#include "umicom/finance/prudential/capital_resource_book.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialCapitalResourceBook b={0}; UmiPrudentialCapitalResource r; UmiCurrency c={"GBP"}; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_resource_init(&r,"R",UMI_PRU_CAPITAL_CET1,100.0,5.0,c)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_capital_resource_book_add(&b,&r)!=UMI_STATUS_OK)return 2; return umi_pru_capital_resource_book_tier_total(&b,UMI_PRU_CAPITAL_CET1)==95.0?0:3; }
