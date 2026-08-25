/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_cycle.c
 *
 * PURPOSE:
 *   Implement define settlement cycle trade-date and settlement-date offsets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_cycle.h"
#include <string.h>
UmiStatus umi_treasury_settlement_cycle_init(UmiTreasurySettlementCycle *value,
    const char *id,
    int32_t settlement_days) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->settlement_days=settlement_days;
    return umi_treasury_settlement_cycle_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_settlement_cycle_valid(const UmiTreasurySettlementCycle *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->settlement_days >= 0 && value->settlement_days <= 30);
}

int32_t umi_treasury_settlement_cycle_offset_days(const UmiTreasurySettlementCycle *value) {
    if (value == NULL) return (int32_t)0;
    return value->settlement_days;
}
