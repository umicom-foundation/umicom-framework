/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_repair.c
 *
 * PURPOSE:
 *   Exercise settlement repair validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_repair.h"
int main(void) {
    UmiTreasurySettlementRepair v;
    if (umi_treasury_settlement_repair_init(&v, "repair", 1U, 3U) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_settlement_repair_retry_allowed(&v))return 2;
    return 0;
}
