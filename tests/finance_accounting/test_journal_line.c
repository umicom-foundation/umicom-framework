/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_journal_line.c
 *
 * PURPOSE:
 *   Exercise journal line validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/journal_line.h"
int main(void) {
    UmiAccountingJournalLine v;
    if(umi_accounting_journal_line_init(&v, "line-1", "1000", 1000, 0)!=UMI_STATUS_OK) return 1;
    if(umi_accounting_journal_line_signed_minor(&v)!=1000) return 2;
    return 0;
}
