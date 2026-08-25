/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_leg.c
 *
 * PURPOSE:
 *   Implement one ordered physical leg within a commodity transport route.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/commodity/transport_leg.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_leg_init(UmiCommodityTransportLeg *value, const char *route_id, uint32_t sequence, const char *origin_location_id, const char *destination_location_id, int64_t planned_departure_ms, int64_t planned_arrival_ms)
{
    UmiStatus status;
    if (value == NULL || sequence == 0U || planned_departure_ms < 0 || planned_arrival_ms <= planned_departure_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->route_id.value, sizeof value->route_id.value, route_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->origin_location_id.value, sizeof value->origin_location_id.value, origin_location_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->destination_location_id.value, sizeof value->destination_location_id.value, destination_location_id);
    if (status != UMI_STATUS_OK) return status;
    value->sequence = sequence;
    value->planned_departure_ms = planned_departure_ms;
    value->planned_arrival_ms = planned_arrival_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_leg_valid(const UmiCommodityTransportLeg *value)
{
    return value != NULL && (umi_commodity_text_valid(value->route_id.value) && value->sequence > 0U && value->planned_arrival_ms > value->planned_departure_ms);
}
