/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_gap.c
 *
 * PURPOSE:
 *   Implement represent a currency liquidity mismatch for a defined horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_gap.h"
#include <string.h>
UmiStatus umi_treasury_liquidity_gap_init(UmiTreasuryLiquidityGap *value,
    const char *id,
    int32_t horizon_days,
    int64_t inflow_minor,
    int64_t outflow_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->horizon_days=horizon_days;
    value->inflow_minor=inflow_minor;
    value->outflow_minor=outflow_minor;
    return umi_treasury_liquidity_gap_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_liquidity_gap_valid(const UmiTreasuryLiquidityGap *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->horizon_days >= 0 && value->inflow_minor >= 0 && value->outflow_minor >= 0);
}

int64_t umi_treasury_liquidity_gap_net_minor(const UmiTreasuryLiquidityGap *value) {
    if (value == NULL) return (int64_t)0;
    return value->inflow_minor - value->outflow_minor;
}
