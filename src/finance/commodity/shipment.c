/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/shipment.c
 *
 * PURPOSE:
 *   Implement a physical shipment and its planned versus delivered quantity.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/shipment.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_shipment_init(UmiCommodityShipment *value, const char *id, const char *contract_id, const char *route_id, int64_t planned_units, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    if (value == NULL || planned_units <= 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->route_id.value, sizeof value->route_id.value, route_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->planned_quantity.unit_code, sizeof value->planned_quantity.unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->planned_quantity.units = planned_units;
    value->planned_quantity.scale = scale;
    value->state = UMI_COMMODITY_SHIPMENT_PLANNED;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_shipment_valid(const UmiCommodityShipment *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->contract_id.value) && value->planned_quantity.units > 0 && value->delivered_units >= 0 && value->delivered_units <= value->planned_quantity.units);
}
