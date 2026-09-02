/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/inventory.c
 *
 * PURPOSE:
 *   Maintain a deterministic bounded submodule inventory.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/inventory.h"

#include <string.h>

/*
 * Initialise repository inventory from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_inventory_init(UmiRepositoryInventory *inventory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL) return;
    (void)memset(inventory, 0, sizeof(*inventory));
    inventory->revision = 1U;
}

/* Provide the find field operation used by this module and its client applications. */
static const UmiRepositorySubmodule *find_field(
    const UmiRepositoryInventory *inventory,
    const char *text,
    int by_path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || text == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->count; ++index) {
        const char *candidate = by_path
            ? inventory->items[index].path
            : inventory->items[index].name;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(candidate, text) == 0) return &inventory->items[index];
    }
    return NULL;
}

/*
 * Provide the repository inventory find path operation used by this module and its client
 * applications.
 */
const UmiRepositorySubmodule *umi_repository_inventory_find_path(
    const UmiRepositoryInventory *inventory,
    const char *path)
{
    return find_field(inventory, path, 1);
}

/*
 * Provide the repository inventory find name operation used by this module and its client
 * applications.
 */
const UmiRepositorySubmodule *umi_repository_inventory_find_name(
    const UmiRepositoryInventory *inventory,
    const char *name)
{
    return find_field(inventory, name, 0);
}

/* Add repository inventory only after its inputs and available capacity have been checked. */
UmiStatus umi_repository_inventory_add(
    UmiRepositoryInventory *inventory,
    const UmiRepositorySubmodule *submodule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL ||
        umi_repository_submodule_validate(submodule) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_repository_inventory_find_path(inventory, submodule->path) != NULL ||
        umi_repository_inventory_find_name(inventory, submodule->name) != NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (inventory->count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    inventory->items[inventory->count++] = *submodule;
    inventory->revision += 1U;
    return UMI_STATUS_OK;
}
