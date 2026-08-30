/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_account.c
 *
 * PURPOSE:
 *   Exercise treasury account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_account.h"
int main(void) {
    UmiTreasuryTreasuryAccount v;
    if (umi_treasury_treasury_account_init(&v, "treasury-gbp", "GBP", 1000, 200) != UMI_STATUS_OK) return 1;
    if (umi_treasury_treasury_account_headroom_minor(&v) != 800) return 2;
    return 0;
}
