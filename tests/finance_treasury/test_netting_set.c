/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_netting_set.c
 *
 * PURPOSE:
 *   Exercise netting set validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_set.h"
int main(void) {
    UmiTreasuryNettingSet v;
    if (umi_treasury_netting_set_init(&v, "netset", 1000, 700) != UMI_STATUS_OK) return 1;
    if(umi_treasury_netting_set_net_minor(&v)!=300)return 2;
    return 0;
}
