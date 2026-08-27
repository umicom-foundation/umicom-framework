/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/sdk_inventory.c
 * PURPOSE: Implement deterministic SDK inventory and default selection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/sdk_inventory.h"

#include <string.h>

void umi_ecosystem_sdk_entry_init(
    UmiEcosystemSdkEntry *entry,
    const char *sdk_id,
    const char *display_name,
    const char *version,
    const char *install_root)
{
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    if (sdk_id != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->sdk_id, sizeof(entry->sdk_id), sdk_id);
    }
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->display_name, sizeof(entry->display_name), display_name);
    }
    if (version != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->version, sizeof(entry->version), version);
    }
    if (install_root != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->install_root, sizeof(entry->install_root), install_root);
    }
    entry->installed = install_root != NULL && install_root[0] != '\0';
    entry->compatibility = UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN;
    entry->evidence = UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
    entry->revision = 1U;
}

void umi_ecosystem_sdk_inventory_init(UmiEcosystemSdkInventory *inventory)
{
    if (inventory == NULL) return;
    (void)memset(inventory, 0, sizeof(*inventory));
    inventory->revision = 1U;
}

UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    size_t index;
    if (inventory == NULL || sdk_id == NULL) return NULL;
    for (index = 0U; index < inventory->entry_count; ++index) {
        if (strcmp(inventory->entries[index].sdk_id, sdk_id) == 0) {
            return &inventory->entries[index];
        }
    }
    return NULL;
}

const UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find_const(
    const UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    size_t index;
    if (inventory == NULL || sdk_id == NULL) return NULL;
    for (index = 0U; index < inventory->entry_count; ++index) {
        if (strcmp(inventory->entries[index].sdk_id, sdk_id) == 0) {
            return &inventory->entries[index];
        }
    }
    return NULL;
}

void umi_ecosystem_sdk_inventory_recalculate(
    UmiEcosystemSdkInventory *inventory)
{
    size_t index;
    if (inventory == NULL) return;
    inventory->installed_count = 0U;
    inventory->compatible_count = 0U;
    inventory->update_count = 0U;
    inventory->unhealthy_count = 0U;
    for (index = 0U; index < inventory->entry_count; ++index) {
        const UmiEcosystemSdkEntry *entry = &inventory->entries[index];
        if (entry->installed) inventory->installed_count++;
        if (entry->compatibility == UMI_ECOSYSTEM_COMPATIBLE) {
            inventory->compatible_count++;
        }
        if (entry->update_available) inventory->update_count++;
        if (entry->installed &&
            (entry->compatibility == UMI_ECOSYSTEM_INCOMPATIBLE ||
             entry->evidence == UMI_ECOSYSTEM_EVIDENCE_REJECTED)) {
            inventory->unhealthy_count++;
        }
    }
}

UmiStatus umi_ecosystem_sdk_inventory_upsert(
    UmiEcosystemSdkInventory *inventory,
    const UmiEcosystemSdkEntry *entry)
{
    UmiEcosystemSdkEntry *existing;
    if (inventory == NULL || entry == NULL || entry->sdk_id[0] == '\0' ||
        entry->display_name[0] == '\0' || entry->version[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (entry->installed && entry->install_root[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    existing = umi_ecosystem_sdk_inventory_find(inventory, entry->sdk_id);
    if (existing != NULL) {
        bool was_default = existing->default_sdk;
        *existing = *entry;
        if (was_default && existing->installed &&
            existing->compatibility == UMI_ECOSYSTEM_COMPATIBLE &&
            existing->evidence != UMI_ECOSYSTEM_EVIDENCE_REJECTED) {
            existing->default_sdk = true;
        }
        existing->revision++;
    } else {
        if (inventory->entry_count >= UMI_ECOSYSTEM_MAX_SDKS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        inventory->entries[inventory->entry_count++] = *entry;
    }
    inventory->revision++;
    umi_ecosystem_sdk_inventory_recalculate(inventory);
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_sdk_inventory_set_default(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    UmiEcosystemSdkEntry *selected;
    size_t index;
    if (inventory == NULL || sdk_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    selected = umi_ecosystem_sdk_inventory_find(inventory, sdk_id);
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;
    if (!selected->installed ||
        selected->compatibility != UMI_ECOSYSTEM_COMPATIBLE ||
        selected->evidence == UMI_ECOSYSTEM_EVIDENCE_REJECTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    for (index = 0U; index < inventory->entry_count; ++index) {
        inventory->entries[index].default_sdk =
            strcmp(inventory->entries[index].sdk_id, sdk_id) == 0;
    }
    inventory->revision++;
    return UMI_STATUS_OK;
}
