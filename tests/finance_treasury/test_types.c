/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_types.c
 *
 * PURPOSE:
 *   Verify shared treasury identifiers, currency parsing and numeric helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/types.h"
#include <string.h>
int main(void) {
    char id[UMI_TREASURY_ID_CAPACITY]; UmiCurrency gbp, gbp2;
    if (!umi_treasury_id_valid("account-1")) return 1;
    if (umi_treasury_id_copy(id, sizeof id, "account-1") != UMI_STATUS_OK) return 2;
    if (strcmp(id,"account-1") != 0) return 3;
    if (umi_treasury_currency_from_code("GBP", &gbp) != UMI_STATUS_OK) return 4;
    if (umi_treasury_currency_from_code("GBP", &gbp2) != UMI_STATUS_OK) return 5;
    if (!umi_treasury_currency_equal(gbp, gbp2)) return 6;
    if (umi_treasury_abs_i64(-42) != 42) return 7;
    return 0;
}
