/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_risk_limit_book.c
 *
 * PURPOSE:
 *   Verify bounded risk limit book registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit_book.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTreasuryRiskLimitBook r; UmiTreasuryRiskLimit item; umi_treasury_risk_limit_book_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_limit_init(&item, "limit", 1000, 800) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_limit_book_add(&r,&item) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_risk_limit_book_find(&r,item.id) == NULL) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_risk_limit_book_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
