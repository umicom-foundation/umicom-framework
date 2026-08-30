/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_failure.c
 *
 * PURPOSE:
 *   Exercise settlement failure validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_failure.h"
int main(void) {
    UmiTreasurySettlementFailure v;
    if (umi_treasury_settlement_failure_init(&v, "fail", 1000, 3U, 1U) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_settlement_failure_aged(&v))return 2;
    return 0;
}
