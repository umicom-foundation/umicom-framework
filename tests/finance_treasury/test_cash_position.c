/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_position.c
 *
 * PURPOSE:
 *   Exercise cash position validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_position.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCashPosition v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_cash_position_init(&v, "pos-1", "USD", 500, 425) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_treasury_cash_position_delta_minor(&v) != -75) return 2;
    return 0;
}
