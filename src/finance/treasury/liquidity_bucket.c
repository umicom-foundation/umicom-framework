/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_bucket.c
 *
 * PURPOSE:
 *   Implement describe a liquidity time bucket and calculate its contractual gap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_bucket.h"
#include <string.h>
UmiStatus umi_treasury_liquidity_bucket_init(UmiTreasuryLiquidityBucket *value,
    const char *id,
    int32_t start_days,
    int32_t end_days,
    int64_t inflow_minor,
    int64_t outflow_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->start_days=start_days;
    value->end_days=end_days;
    value->inflow_minor=inflow_minor;
    value->outflow_minor=outflow_minor;
    return umi_treasury_liquidity_bucket_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_liquidity_bucket_valid(const UmiTreasuryLiquidityBucket *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->start_days >= 0 && value->end_days >= value->start_days && value->inflow_minor >= 0 && value->outflow_minor >= 0);
}

int64_t umi_treasury_liquidity_bucket_gap_minor(const UmiTreasuryLiquidityBucket *value) {
    if (value == NULL) return (int64_t)0;
    return value->inflow_minor - value->outflow_minor;
}
