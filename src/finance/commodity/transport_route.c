/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_route.c
 *
 * PURPOSE:
 *   Implement an auditable commodity logistics route between physical locations.
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

#include "umicom/finance/commodity/transport_route.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_route_init(UmiCommodityTransportRoute *value, const char *id, const char *origin_location_id, const char *destination_location_id, const char *mode_code)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->origin_location_id.value, sizeof value->origin_location_id.value, origin_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->destination_location_id.value, sizeof value->destination_location_id.value, destination_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->mode_code, sizeof value->mode_code, mode_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_route_valid(const UmiCommodityTransportRoute *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->origin_location_id.value) && umi_commodity_text_valid(value->destination_location_id.value) && strcmp(value->origin_location_id.value, value->destination_location_id.value) != 0 && value->active);
}
