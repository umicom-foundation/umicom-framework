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
int main(void) {
    UmiTreasurySecuritiesLendingContract v;
    if (umi_treasury_securities_lending_contract_init(&v, "sl", 100, 10000, 50U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_securities_lending_contract_annual_fee_minor(&v)!=50)return 2;
    return 0;
}
