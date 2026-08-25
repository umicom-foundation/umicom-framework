/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_buffer.c
 *
 * PURPOSE:
 *   Implement compare available liquidity against a policy buffer requirement.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_buffer.h"
#include <string.h>
UmiStatus umi_treasury_liquidity_buffer_init(UmiTreasuryLiquidityBuffer *value,
    const char *id,
    int64_t available_minor,
    int64_t required_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->available_minor=available_minor;
    value->required_minor=required_minor;
    return umi_treasury_liquidity_buffer_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_liquidity_buffer_valid(const UmiTreasuryLiquidityBuffer *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->available_minor >= 0 && value->required_minor >= 0);
}

int64_t umi_treasury_liquidity_buffer_surplus_minor(const UmiTreasuryLiquidityBuffer *value) {
    if (value == NULL) return (int64_t)0;
    return value->available_minor - value->required_minor;
}
