/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/inventory.h
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
#ifndef UMICOM_REPOSITORY_INVENTORY_H
#define UMICOM_REPOSITORY_INVENTORY_H
#include "umicom/repository/submodule.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository inventory data shared with callers of this public contract.
 */
typedef struct UmiRepositoryInventory {
    UmiRepositorySubmodule items[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiRepositoryInventory;
/**
 * Initialise repository inventory from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_inventory_init(UmiRepositoryInventory *inventory);
/**
 * Add repository inventory only after its inputs and available capacity have been checked.
 */
UmiStatus umi_repository_inventory_add(
    UmiRepositoryInventory *inventory,
    const UmiRepositorySubmodule *submodule);
/**
 * Provide the repository inventory find path operation used by this module and its client
 * applications.
 */
const UmiRepositorySubmodule *umi_repository_inventory_find_path(
    const UmiRepositoryInventory *inventory,
    const char *path);
/**
 * Provide the repository inventory find name operation used by this module and its client
 * applications.
 */
const UmiRepositorySubmodule *umi_repository_inventory_find_name(
    const UmiRepositoryInventory *inventory,
    const char *name);
#ifdef __cplusplus
}
#endif
#endif
