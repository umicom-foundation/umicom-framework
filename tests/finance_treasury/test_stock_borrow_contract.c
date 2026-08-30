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
int main(void) {
    UmiTreasuryStockBorrowContract v;
    if (umi_treasury_stock_borrow_contract_init(&v, "borrow", 50, 20000, 100U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_stock_borrow_contract_annual_fee_minor(&v)!=200)return 2;
    return 0;
}
