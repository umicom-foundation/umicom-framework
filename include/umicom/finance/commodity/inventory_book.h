/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/commodity/inventory_book.h
 *
 * PURPOSE:
 *   Define a bounded book of facility-level commodity inventory records.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_BOOK_H
#define INCLUDE_UMICOM_FINANCE_COMMODITY_INVENTORY_BOOK_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/finance/commodity/types.h"
#include "umicom/finance/commodity/storage_inventory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCommodityInventoryBook {
    UmiCommodityStorageInventory items[UMI_COMMODITY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiCommodityInventoryBook;

/* Reset a bounded catalogue before loading reference data. */
void umi_commodity_inventory_book_init(UmiCommodityInventoryBook *catalogue);

/* Add a unique item and advance the deterministic catalogue revision. */
UmiStatus umi_commodity_inventory_book_add(UmiCommodityInventoryBook *catalogue, const UmiCommodityStorageInventory *item);

/* Resolve an item by its stable Framework identifier. */
const UmiCommodityStorageInventory *umi_commodity_inventory_book_find(const UmiCommodityInventoryBook *catalogue, const char *id);

#ifdef __cplusplus
}
#endif

#endif
