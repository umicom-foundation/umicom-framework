/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_snapshot.c
 *
 * PURPOSE:
 *   Exercise treasury snapshot validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_snapshot.h"
int main(void) {
    UmiTreasuryTreasurySnapshot v;
    if (umi_treasury_treasury_snapshot_init(&v, "snapshot", 1000, -250, 400, 600) != UMI_STATUS_OK) return 1;
    if(umi_treasury_treasury_snapshot_net_liquidity_minor(&v)!=750)return 2;
    return 0;
}
