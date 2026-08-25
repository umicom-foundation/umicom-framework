/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_window.c
 *
 * PURPOSE:
 *   Exercise settlement window validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_window.h"
int main(void) {
    UmiTreasurySettlementWindow v;
    if (umi_treasury_settlement_window_init(&v, "window", 100, 250) != UMI_STATUS_OK) return 1;
    if(umi_treasury_settlement_window_duration_millis(&v)!=150)return 2;
    return 0;
}
