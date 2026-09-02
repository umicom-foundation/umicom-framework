/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_window.c
 *
 * PURPOSE:
 *   Exercise settlement window validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_window.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasurySettlementWindow v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_settlement_window_init(&v, "window", 100, 250) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_settlement_window_duration_millis(&v)!=150)return 2;
    return 0;
}
