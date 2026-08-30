/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/inventory.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_INVENTORY_H
#define INCLUDE_UMICOM_DELIVERY_INVENTORY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiInventoryEntry {
    char path[UMI_DELIVERY_PATH_CAPACITY];
    uint64_t size_bytes;
} UmiInventoryEntry;
typedef struct UmiDeliveryInventory {
    UmiInventoryEntry entries[UMI_DELIVERY_MAX_COMPONENTS];
    size_t count;
} UmiDeliveryInventory;
void umi_delivery_inventory_init(UmiDeliveryInventory *inventory);
UmiStatus umi_delivery_inventory_add(UmiDeliveryInventory *inventory,
                                     const char *path,
                                     uint64_t size_bytes);
uint64_t umi_delivery_inventory_total_bytes(const UmiDeliveryInventory *inventory);

#ifdef __cplusplus
}
#endif

#endif
