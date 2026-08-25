/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/securities_lending_contract.c
 *
 * PURPOSE:
 *   Implement model securities lending quantity, collateral value and fee rate.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_lending_contract.h"
#include <string.h>
UmiStatus umi_treasury_securities_lending_contract_init(UmiTreasurySecuritiesLendingContract *value,
    const char *id,
    int64_t quantity,
    int64_t collateral_minor,
    uint32_t fee_bps) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->quantity=quantity;
    value->collateral_minor=collateral_minor;
    value->fee_bps=fee_bps;
    return umi_treasury_securities_lending_contract_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_securities_lending_contract_valid(const UmiTreasurySecuritiesLendingContract *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->quantity > 0 && value->collateral_minor >= 0 && value->fee_bps <= 10000U);
}

int64_t umi_treasury_securities_lending_contract_annual_fee_minor(const UmiTreasurySecuritiesLendingContract *value) {
    if (value == NULL) return (int64_t)0;
    return (value->collateral_minor * (int64_t)value->fee_bps) / 10000;
}
