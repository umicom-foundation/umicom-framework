/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/storage_inventory.c
 *
 * PURPOSE:
 *   Track available and reserved commodity inventory at one storage facility.
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

#include "umicom/finance/commodity/storage_inventory.h"

#include <string.h>

/* Initialise bounded physical inventory state. */
UmiStatus umi_commodity_storage_inventory_init(UmiCommodityStorageInventory *value, const char *facility_id, const char *commodity_id, int64_t available_units, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    if (value == NULL || available_units < 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->facility_id.value, sizeof value->facility_id.value, facility_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->unit_code, sizeof value->unit_code, unit_code);
    if (status != UMI_STATUS_OK) return status;
    value->available_units = available_units;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Reservation is explicit so delivery planning cannot silently over-allocate stock. */
UmiStatus umi_commodity_storage_inventory_reserve(UmiCommodityStorageInventory *value, int64_t units)
{
    if (value == NULL || units <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (units > umi_commodity_storage_inventory_free_units(value)) return UMI_STATUS_UNAVAILABLE;
    value->reserved_units += units;
    return UMI_STATUS_OK;
}

/* Release only quantities that are currently reserved. */
UmiStatus umi_commodity_storage_inventory_release(UmiCommodityStorageInventory *value, int64_t units)
{
    if (value == NULL || units <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (units > value->reserved_units) return UMI_STATUS_INVALID_STATE;
    value->reserved_units -= units;
    return UMI_STATUS_OK;
}

/* Free inventory is a derived value and therefore cannot drift independently. */
int64_t umi_commodity_storage_inventory_free_units(const UmiCommodityStorageInventory *value)
{
    if (value == NULL) return 0;
    return value->available_units - value->reserved_units;
}
