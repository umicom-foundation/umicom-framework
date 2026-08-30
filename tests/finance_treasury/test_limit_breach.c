/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_limit_breach.c
 *
 * PURPOSE:
 *   Exercise limit breach validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_breach.h"
int main(void) {
    UmiTreasuryLimitBreach v;
    if (umi_treasury_limit_breach_init(&v, "breach", 50, false) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_limit_breach_open(&v))return 2;
    return 0;
}
