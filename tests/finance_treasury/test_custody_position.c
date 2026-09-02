/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_custody_position.c
 *
 * PURPOSE:
 *   Exercise custody position validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_position.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCustodyPosition v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_custody_position_init(&v, "pos", 100, 20, 10) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_custody_position_projected_quantity(&v)!=110)return 2;
    return 0;
}
