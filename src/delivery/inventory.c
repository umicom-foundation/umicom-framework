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

/*
 * The inventory is a simple source of truth for what files are expected to appear in one package.
 */

#include "umicom/delivery/inventory.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise delivery inventory from caller-provided values so later operations receive a
 * known state.
 */
void umi_delivery_inventory_init(UmiDeliveryInventory *inventory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory != NULL) (void)memset(inventory, 0, sizeof(*inventory));
}
/* Add delivery inventory only after its inputs and available capacity have been checked. */
UmiStatus umi_delivery_inventory_add(UmiDeliveryInventory *inventory,
                                     const char *path,
                                     uint64_t size_bytes)
{
    UmiInventoryEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inventory->count >= UMI_DELIVERY_MAX_COMPONENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &inventory->entries[inventory->count];
    (void)memset(entry, 0, sizeof(*entry));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(entry->path, sizeof(entry->path), path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry->size_bytes = size_bytes;
    ++inventory->count;
    return UMI_STATUS_OK;
}
/*
 * Provide the delivery inventory total bytes operation used by this module and its client
 * applications.
 */
uint64_t umi_delivery_inventory_total_bytes(const UmiDeliveryInventory *inventory)
{
    size_t i;
    uint64_t total = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < inventory->count; ++i) total += inventory->entries[i].size_bytes;
    return total;
}
