/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_cycle.c
 *
 * PURPOSE:
 *   Exercise settlement cycle validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_cycle.h"
int main(void) {
    UmiTreasurySettlementCycle v;
    if (umi_treasury_settlement_cycle_init(&v, "T2", 2) != UMI_STATUS_OK) return 1;
    if(umi_treasury_settlement_cycle_offset_days(&v)!=2)return 2;
    return 0;
}
