/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_event.c
 *
 * PURPOSE:
 *   Exercise treasury event validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryTreasuryEvent v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_treasury_event_init(&v, "event", 1U, 1000) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_treasury_event_event_sequence(&v)!=1U)return 2;
    return 0;
}
