/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT
 * Private immutable storage for release registration inventories. */
#ifndef UMICOM_RELEASE_INVENTORY_INTERNAL_H
#define UMICOM_RELEASE_INVENTORY_INTERNAL_H
#include "umicom/distribution/runtime/inventory.h"
struct UmiReleaseInventory {
    char *storage;
    char *producer, *generation, *sourceRoot, *buildRoot, *configuration;
    size_t count;
    UmiReleaseInventoryRecord *records;
};
#endif
