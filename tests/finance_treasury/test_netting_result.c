/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_netting_result.c
 *
 * PURPOSE:
 *   Exercise netting result validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_result.h"
int main(void) {
    UmiTreasuryNettingResult v;
    if (umi_treasury_netting_result_init(&v, "netres", 1000, 250) != UMI_STATUS_OK) return 1;
    if(umi_treasury_netting_result_reduction_minor(&v)!=750)return 2;
    return 0;
}
