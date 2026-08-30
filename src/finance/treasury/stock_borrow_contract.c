/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/stock_borrow_contract.c
 *
 * PURPOSE:
 *   Implement model stock borrow quantity, mark value and borrow fee.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/stock_borrow_contract.h"
#include <string.h>
UmiStatus umi_treasury_stock_borrow_contract_init(UmiTreasuryStockBorrowContract *value,
    const char *id,
    int64_t quantity,
    int64_t mark_value_minor,
    uint32_t borrow_fee_bps) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->quantity=quantity;
    value->mark_value_minor=mark_value_minor;
    value->borrow_fee_bps=borrow_fee_bps;
    return umi_treasury_stock_borrow_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_stock_borrow_contract_valid(const UmiTreasuryStockBorrowContract *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->quantity > 0 && value->mark_value_minor >= 0 && value->borrow_fee_bps <= 10000U);
}

int64_t umi_treasury_stock_borrow_contract_annual_fee_minor(const UmiTreasuryStockBorrowContract *value) {
    if (value == NULL) return (int64_t)0;
    return (value->mark_value_minor * (int64_t)value->borrow_fee_bps) / 10000;
}
