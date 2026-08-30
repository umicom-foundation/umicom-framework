/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/storage_facility.c
 *
 * PURPOSE:
 *   Implement storage capacity at a physical commodity location.
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

#include "umicom/finance/commodity/storage_facility.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_storage_facility_init(UmiCommodityStorageFacility *value, const char *id, const char *location_id, int64_t capacity_units, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    if (value == NULL || capacity_units < 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->location_id.value, sizeof value->location_id.value, location_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->capacity.unit_code, sizeof value->capacity.unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->capacity.units = capacity_units;
    value->capacity.scale = scale;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_storage_facility_valid(const UmiCommodityStorageFacility *value)
{
    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->location_id.value) && value->capacity.units >= 0 && umi_commodity_text_valid(value->capacity.unit_code) && value->active);
}
