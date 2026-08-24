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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_INVENTORY_H
#define UMICOM_REPOSITORY_INVENTORY_H
#include "umicom/repository/submodule.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRepositoryInventory {
    UmiRepositorySubmodule items[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiRepositoryInventory;
void umi_repository_inventory_init(UmiRepositoryInventory *inventory);
UmiStatus umi_repository_inventory_add(
    UmiRepositoryInventory *inventory,
    const UmiRepositorySubmodule *submodule);
const UmiRepositorySubmodule *umi_repository_inventory_find_path(
    const UmiRepositoryInventory *inventory,
    const char *path);
const UmiRepositorySubmodule *umi_repository_inventory_find_name(
    const UmiRepositoryInventory *inventory,
    const char *name);
#ifdef __cplusplus
}
#endif
#endif
