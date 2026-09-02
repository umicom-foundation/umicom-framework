/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_subledger.c
 *
 * PURPOSE:
 *   Exercise subledger validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/subledger.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingSubledger v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_subledger_init(&v, "sub-1", "1200", 3000, 500)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_subledger_net_minor(&v)!=2500) return 2;
    return 0;
}
