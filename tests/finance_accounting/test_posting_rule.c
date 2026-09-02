/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_posting_rule.c
 *
 * PURPOSE:
 *   Exercise posting rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/posting_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingPostingRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_posting_rule_init(&v, "rule-1", "CASH_RECEIPT", "1000", "4000", true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_posting_rule_valid(&v)) return 2;
    return 0;
}
