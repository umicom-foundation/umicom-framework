/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_journal_line.c
 *
 * PURPOSE:
 *   Exercise journal line validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_line.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingJournalLine v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_journal_line_init(&v, "line-1", "1000", 1000, 0)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_journal_line_signed_minor(&v)!=1000) return 2;
    return 0;
}
