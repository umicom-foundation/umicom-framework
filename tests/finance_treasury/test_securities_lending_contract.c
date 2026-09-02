/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_securities_lending_contract.c
 *
 * PURPOSE:
 *   Exercise securities lending contract validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_lending_contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasurySecuritiesLendingContract v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_securities_lending_contract_init(&v, "sl", 100, 10000, 50U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_securities_lending_contract_annual_fee_minor(&v)!=50)return 2;
    return 0;
}
