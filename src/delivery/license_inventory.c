/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/license_inventory.c
 *
 * PURPOSE:
 *   Track third-party licence identifiers and notice paths included in a release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Keeping licences structured makes packaging and compliance checks repeatable instead of relying on copied text files.
 */

#include "umicom/delivery/license_inventory.h"
#include "delivery_internal.h"
#include <string.h>

/*
 * Initialise license inventory from caller-provided values so later operations receive a
 * known state.
 */
void umi_license_inventory_init(UmiLicenseInventory *inventory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory != NULL) (void)memset(inventory, 0, sizeof(*inventory));
}

/* Add license inventory only after its inputs and available capacity have been checked. */
UmiStatus umi_license_inventory_add(UmiLicenseInventory *inventory,
                                    const char *component,
                                    const char *license_id,
                                    const char *notice_path)
{
    UmiLicenseEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || component == NULL || license_id == NULL ||
        notice_path == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inventory->count >= UMI_DELIVERY_MAX_LICENSES) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &inventory->entries[inventory->count];
    (void)memset(entry, 0, sizeof(*entry));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(entry->component, sizeof(entry->component), component) != UMI_STATUS_OK ||
        umi_delivery_copy_text(entry->license_id, sizeof(entry->license_id), license_id) != UMI_STATUS_OK ||
        umi_delivery_copy_text(entry->notice_path, sizeof(entry->notice_path), notice_path) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    ++inventory->count;
    return UMI_STATUS_OK;
}

/*
 * Provide the license inventory contains operation used by this module and its client
 * applications.
 */
int umi_license_inventory_contains(const UmiLicenseInventory *inventory,
                                   const char *component)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || component == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < inventory->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(inventory->entries[i].component, component) == 0) return 1;
    }
    return 0;
}
