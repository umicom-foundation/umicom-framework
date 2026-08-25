/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/inventory_lot.c
 *
 * PURPOSE:
 *   Implement an auditable physical inventory lot and its available quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/inventory_lot.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_inventory_lot_init(UmiCommodityInventoryLot *value, const char *id, const char *commodity_id, const char *facility_id, int64_t units, int32_t scale, const char *unit_code, int64_t received_time_ms)
{
    UmiStatus status;
    if (value == NULL || units < 0 || scale < 0 || received_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->facility_id.value, sizeof value->facility_id.value, facility_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->quantity.unit_code, sizeof value->quantity.unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->quantity.units = units;
    value->quantity.scale = scale;
    value->received_time_ms = received_time_ms;
    value->quality_accepted = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_inventory_lot_valid(const UmiCommodityInventoryLot *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->commodity_id.value) && umi_commodity_text_valid(value->facility_id.value) && value->quantity.units >= 0 && value->quality_accepted);
}
