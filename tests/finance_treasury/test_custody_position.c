/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_custody_position.c
 *
 * PURPOSE:
 *   Exercise custody position validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/custody_position.h"
int main(void) {
    UmiTreasuryCustodyPosition v;
    if (umi_treasury_custody_position_init(&v, "pos", 100, 20, 10) != UMI_STATUS_OK) return 1;
    if(umi_treasury_custody_position_projected_quantity(&v)!=110)return 2;
    return 0;
}
