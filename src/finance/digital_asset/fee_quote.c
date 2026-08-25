/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/fee_quote.c
 *
 * PURPOSE:
 *   Implement a time-bounded fee quote for a digital-asset network.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/fee_quote.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_fee_quote_init(UmiDigitalFeeQuote *value, const char *network_id, int64_t fee_units, int32_t scale, const char *asset_symbol, int64_t expires_time_ms)
{
    UmiStatus status;
    if (value == NULL || fee_units < 0 || scale < 0 || expires_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->estimated_fee.asset_symbol, sizeof value->estimated_fee.asset_symbol, asset_symbol);
    if (status != UMI_STATUS_OK) return status;
    value->estimated_fee.units = fee_units;
    value->estimated_fee.scale = scale;
    value->expires_time_ms = expires_time_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_fee_quote_valid(const UmiDigitalFeeQuote *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && value->estimated_fee.units >= 0 && value->expires_time_ms >= 0);
}
