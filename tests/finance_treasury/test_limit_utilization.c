/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_limit_utilization.c
 *
 * PURPOSE:
 *   Exercise limit utilization validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/limit_utilization.h"
int main(void) {
    UmiTreasuryLimitUtilization v;
    if (umi_treasury_limit_utilization_init(&v, "util", 750, 1000) != UMI_STATUS_OK) return 1;
    if(umi_treasury_limit_utilization_utilization_bps(&v)!=7500U)return 2;
    return 0;
}
