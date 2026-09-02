/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_stock_borrow_contract.c
 *
 * PURPOSE:
 *   Exercise stock borrow contract validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/stock_borrow_contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryStockBorrowContract v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_stock_borrow_contract_init(&v, "borrow", 50, 20000, 100U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_stock_borrow_contract_annual_fee_minor(&v)!=200)return 2;
    return 0;
}
