/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_netting.c
 *
 * PURPOSE:
 *   Exercise settlement netting validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_netting.h"
int main(void) {
    UmiTreasurySettlementNetting v;
    if (umi_treasury_settlement_netting_init(&v, "net", 800, 500) != UMI_STATUS_OK) return 1;
    if(umi_treasury_settlement_netting_net_minor(&v)!=-300)return 2;
    return 0;
}
