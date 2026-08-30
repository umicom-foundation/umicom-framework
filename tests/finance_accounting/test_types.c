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
int main(void) {
    UmiFinancialId id; UmiCurrency gbp,gbp2;
    if(umi_accounting_id_assign(&id,"id-1")!=UMI_STATUS_OK) return 1;
    if(!umi_financial_id_is_valid(&id)) return 2;
    if(umi_accounting_currency_from_code("GBP",&gbp)!=UMI_STATUS_OK) return 3;
    if(umi_accounting_currency_from_code("GBP",&gbp2)!=UMI_STATUS_OK) return 4;
    if(!umi_accounting_currency_equal(gbp,gbp2)) return 5;
    if(umi_accounting_abs_i64(-42)!=42) return 6;
    return 0;
}
