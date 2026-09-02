/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_liquidity_buffer.c
 *
 * PURPOSE:
 *   Exercise liquidity buffer validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_buffer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryLiquidityBuffer v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_liquidity_buffer_init(&v, "buffer", 1200, 900) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_liquidity_buffer_surplus_minor(&v)!=300)return 2;
    return 0;
}
