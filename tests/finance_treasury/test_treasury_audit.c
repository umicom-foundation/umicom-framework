/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_audit.c
 *
 * PURPOSE:
 *   Exercise treasury audit validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_audit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryTreasuryAudit v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_treasury_audit_init(&v, "audit", "operator", 7U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_treasury_audit_sequenced(&v))return 2;
    return 0;
}
