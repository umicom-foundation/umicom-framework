/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_types.c
 *
 * PURPOSE:
 *   Verify shared accounting identifiers, currencies and signed amount helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId id; UmiCurrency gbp,gbp2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_id_assign(&id,"id-1")!=UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_financial_id_is_valid(&id)) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_currency_from_code("GBP",&gbp)!=UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_currency_from_code("GBP",&gbp2)!=UMI_STATUS_OK) return 4;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_accounting_currency_equal(gbp,gbp2)) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_accounting_abs_i64(-42)!=42) return 6;
    return 0;
}
