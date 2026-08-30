/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/inventory.c
 *
 * PURPOSE:
 *   Track staged files and their sizes as a package inventory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The inventory is a simple source of truth for what files are expected to appear in one package.
 */

#include "umicom/delivery/inventory.h"
#include "delivery_internal.h"
#include <string.h>
void umi_delivery_inventory_init(UmiDeliveryInventory *inventory)
{
    if (inventory != NULL) (void)memset(inventory, 0, sizeof(*inventory));
}
UmiStatus umi_delivery_inventory_add(UmiDeliveryInventory *inventory,
                                     const char *path,
                                     uint64_t size_bytes)
{
    UmiInventoryEntry *entry;
    if (inventory == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (inventory->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &inventory->entries[inventory->count];
    (void)memset(entry, 0, sizeof(*entry));
    if (umi_delivery_copy_text(entry->path, sizeof(entry->path), path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry->size_bytes = size_bytes;
    ++inventory->count;
    return UMI_STATUS_OK;
}
uint64_t umi_delivery_inventory_total_bytes(const UmiDeliveryInventory *inventory)
{
    size_t i;
    uint64_t total = 0U;
    if (inventory == NULL) return 0U;
    for (i = 0U; i < inventory->count; ++i) total += inventory->entries[i].size_bytes;
    return total;
}
