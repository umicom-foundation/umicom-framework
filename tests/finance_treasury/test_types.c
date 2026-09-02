/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_types.c
 *
 * PURPOSE:
 *   Verify shared treasury identifiers, currency parsing and numeric helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char id[UMI_TREASURY_ID_CAPACITY]; UmiCurrency gbp, gbp2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_treasury_id_valid("account-1")) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_id_copy(id, sizeof id, "account-1") != UMI_STATUS_OK) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(id,"account-1") != 0) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_currency_from_code("GBP", &gbp) != UMI_STATUS_OK) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_currency_from_code("GBP", &gbp2) != UMI_STATUS_OK) return 5;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_treasury_currency_equal(gbp, gbp2)) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_treasury_abs_i64(-42) != 42) return 7;
    return 0;
}
