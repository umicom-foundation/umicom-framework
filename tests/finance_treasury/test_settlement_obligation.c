/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_obligation.c
 *
 * PURPOSE:
 *   Exercise settlement obligation validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_obligation.h"
int main(void) {
    UmiTreasurySettlementObligation v;
    if (umi_treasury_settlement_obligation_init(&v, "settle-1", 1000, 50, UMI_TREASURY_SETTLEMENT_SETTLED) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_settlement_obligation_complete(&v))return 2;
    return 0;
}
