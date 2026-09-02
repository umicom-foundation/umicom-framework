/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_double_entry_validation.c
 *
 * PURPOSE:
 *   Exercise double entry validation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/double_entry_validation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingDoubleEntryValidation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_double_entry_validation_init(&v, "valid-1", 1000, 1000, 2U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_double_entry_validation_balanced(&v)) return 2;
    return 0;
}
