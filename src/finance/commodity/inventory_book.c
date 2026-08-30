/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/inventory_book.c
 *
 * PURPOSE:
 *   Implement a bounded book of facility-level commodity inventory records.
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

#include "umicom/finance/commodity/inventory_book.h"

#include <string.h>

/* Reset catalogue state without heap allocation. */
void umi_commodity_inventory_book_init(UmiCommodityInventoryBook *catalogue)
{
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof *catalogue);
    }
}

/* Reject duplicates and preserve insertion order for reproducible snapshots. */
UmiStatus umi_commodity_inventory_book_add(UmiCommodityInventoryBook *catalogue, const UmiCommodityStorageInventory *item)
{
    size_t index;
    if (catalogue == NULL || item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].facility_id.value, (*item).facility_id.value) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (catalogue->count >= UMI_COMMODITY_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    catalogue->items[catalogue->count++] = *item;
    catalogue->revision++;
    return UMI_STATUS_OK;
}

/* Resolve reference data without exposing catalogue storage mutability. */
const UmiCommodityStorageInventory *umi_commodity_inventory_book_find(const UmiCommodityInventoryBook *catalogue, const char *id)
{
    size_t index;
    if (catalogue == NULL || id == NULL) {
        return NULL;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].facility_id.value, id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}
