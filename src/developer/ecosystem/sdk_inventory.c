/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/sdk_inventory.c
 * PURPOSE: Implement deterministic SDK inventory and default selection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/sdk_inventory.h"

#include <string.h>

/*
 * Initialise ecosystem sdk entry from caller-provided values so later operations receive a
 * known state.
 */
void umi_ecosystem_sdk_entry_init(
    UmiEcosystemSdkEntry *entry,
    const char *sdk_id,
    const char *display_name,
    const char *version,
    const char *install_root)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) return;
    (void)memset(entry, 0, sizeof(*entry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sdk_id != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->sdk_id, sizeof(entry->sdk_id), sdk_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (display_name != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->display_name, sizeof(entry->display_name), display_name);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (version != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->version, sizeof(entry->version), version);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (install_root != NULL) {
        (void)umi_ecosystem_copy_text(
            entry->install_root, sizeof(entry->install_root), install_root);
    }
    entry->installed = install_root != NULL && install_root[0] != '\0';
    entry->compatibility = UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN;
    entry->evidence = UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
    entry->revision = 1U;
}

/*
 * Initialise ecosystem sdk inventory from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_sdk_inventory_init(UmiEcosystemSdkInventory *inventory)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL) return;
    (void)memset(inventory, 0, sizeof(*inventory));
    inventory->revision = 1U;
}

/*
 * Find ecosystem sdk inventory while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || sdk_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->entry_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(inventory->entries[index].sdk_id, sdk_id) == 0) {
            return &inventory->entries[index];
        }
    }
    return NULL;
}

/*
 * Provide the ecosystem sdk inventory find const operation used by this module and its
 * client applications.
 */
const UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find_const(
    const UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || sdk_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->entry_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(inventory->entries[index].sdk_id, sdk_id) == 0) {
            return &inventory->entries[index];
        }
    }
    return NULL;
}

/*
 * Provide the ecosystem sdk inventory recalculate operation used by this module and its
 * client applications.
 */
void umi_ecosystem_sdk_inventory_recalculate(
    UmiEcosystemSdkInventory *inventory)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL) return;
    inventory->installed_count = 0U;
    inventory->compatible_count = 0U;
    inventory->update_count = 0U;
    inventory->unhealthy_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->entry_count; ++index) {
        const UmiEcosystemSdkEntry *entry = &inventory->entries[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->installed) inventory->installed_count++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->compatibility == UMI_ECOSYSTEM_COMPATIBLE) {
            inventory->compatible_count++;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->update_available) inventory->update_count++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->installed &&
            (entry->compatibility == UMI_ECOSYSTEM_INCOMPATIBLE ||
             entry->evidence == UMI_ECOSYSTEM_EVIDENCE_REJECTED)) {
            inventory->unhealthy_count++;
        }
    }
}

/*
 * Provide the ecosystem sdk inventory upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_sdk_inventory_upsert(
    UmiEcosystemSdkInventory *inventory,
    const UmiEcosystemSdkEntry *entry)
{
    UmiEcosystemSdkEntry *existing;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || entry == NULL || entry->sdk_id[0] == '\0' ||
        entry->display_name[0] == '\0' || entry->version[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (entry->installed && entry->install_root[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }
    existing = umi_ecosystem_sdk_inventory_find(inventory, entry->sdk_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) {
        bool was_default = existing->default_sdk;
        *existing = *entry;
        /* Apply this branch only when its contract condition is satisfied. */
        if (was_default && existing->installed &&
            existing->compatibility == UMI_ECOSYSTEM_COMPATIBLE &&
            existing->evidence != UMI_ECOSYSTEM_EVIDENCE_REJECTED) {
            existing->default_sdk = true;
        }
        existing->revision++;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (inventory->entry_count >= UMI_ECOSYSTEM_MAX_SDKS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        inventory->entries[inventory->entry_count++] = *entry;
    }
    inventory->revision++;
    umi_ecosystem_sdk_inventory_recalculate(inventory);
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem sdk inventory set default operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_sdk_inventory_set_default(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id)
{
    UmiEcosystemSdkEntry *selected;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (inventory == NULL || sdk_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    selected = umi_ecosystem_sdk_inventory_find(inventory, sdk_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selected == NULL) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!selected->installed ||
        selected->compatibility != UMI_ECOSYSTEM_COMPATIBLE ||
        selected->evidence == UMI_ECOSYSTEM_EVIDENCE_REJECTED) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < inventory->entry_count; ++index) {
        inventory->entries[index].default_sdk =
            strcmp(inventory->entries[index].sdk_id, sdk_id) == 0;
    }
    inventory->revision++;
    return UMI_STATUS_OK;
}
