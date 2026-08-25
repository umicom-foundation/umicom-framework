/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/digital_market.c
 *
 * PURPOSE:
 *   Implement a digital-asset market pair that can be routed through canonical trading services.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/digital_market.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_digital_market_init(UmiDigitalMarket *value, const char *id, const char *base_asset_id, const char *quote_asset_id, const char *venue, int64_t minimum_quantity_units)
{
    UmiStatus status;
    if (value == NULL || minimum_quantity_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->base_asset_id.value, sizeof value->base_asset_id.value, base_asset_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->quote_asset_id.value, sizeof value->quote_asset_id.value, quote_asset_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->venue, sizeof value->venue, venue);
    if (status != UMI_STATUS_OK) return status;
    value->minimum_quantity_units = minimum_quantity_units;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_digital_market_valid(const UmiDigitalMarket *value)
{
    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->base_asset_id.value) && umi_digital_asset_text_valid(value->quote_asset_id.value) && strcmp(value->base_asset_id.value, value->quote_asset_id.value) != 0 && value->active);
}
