/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/storage_inventory.h
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

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_INVENTORY_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_STORAGE_INVENTORY_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityStorageInventory {
    UmiCommodityId facility_id;
    UmiCommodityId commodity_id;
    int64_t available_units;
    int64_t reserved_units;
    int32_t scale;
    char unit_code[UMI_COMMODITY_CODE_CAPACITY];
} UmiCommodityStorageInventory;

/* Initialise physical inventory totals at a facility. */
UmiStatus umi_commodity_storage_inventory_init(UmiCommodityStorageInventory *value, const char *facility_id, const char *commodity_id, int64_t available_units, int32_t scale, const char *unit_code);

/* Reserve inventory without allowing commitments to exceed availability. */
UmiStatus umi_commodity_storage_inventory_reserve(UmiCommodityStorageInventory *value, int64_t units);

/* Release a prior reservation back to available inventory. */
UmiStatus umi_commodity_storage_inventory_release(UmiCommodityStorageInventory *value, int64_t units);

/* Return unreserved physical units currently available for allocation. */
int64_t umi_commodity_storage_inventory_free_units(const UmiCommodityStorageInventory *value);

#ifdef __cplusplus
}
#endif

#endif
