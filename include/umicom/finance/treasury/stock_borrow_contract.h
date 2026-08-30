/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/stock_borrow_contract.h
 *
 * PURPOSE:
 *   Model stock borrow quantity, mark value and borrow fee.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_STOCK_BORROW_CONTRACT_H
#define UMICOM_FINANCE_TREASURY_STOCK_BORROW_CONTRACT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryStockBorrowContract {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t quantity;
    int64_t mark_value_minor;
    uint32_t borrow_fee_bps;
} UmiTreasuryStockBorrowContract;
UmiStatus umi_treasury_stock_borrow_contract_init(UmiTreasuryStockBorrowContract *value,
    const char *id,
    int64_t quantity,
    int64_t mark_value_minor,
    uint32_t borrow_fee_bps);
bool umi_treasury_stock_borrow_contract_valid(const UmiTreasuryStockBorrowContract *value);
int64_t umi_treasury_stock_borrow_contract_annual_fee_minor(const UmiTreasuryStockBorrowContract *value);
#ifdef __cplusplus
}
#endif
#endif
