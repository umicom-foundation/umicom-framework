/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/sdk_inventory.h
 * PURPOSE: Project installed Framework SDK/runtime evidence for SDK-manager UI.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_SDK_INVENTORY_H
#define UMICOM_DEVELOPER_ECOSYSTEM_SDK_INVENTORY_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem sdk entry data shared with callers of this public contract.
 */
typedef struct UmiEcosystemSdkEntry {
    char sdk_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char display_name[UMI_ECOSYSTEM_NAME_CAPACITY];
    char version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char available_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char install_root[UMI_ECOSYSTEM_PATH_CAPACITY];
    char platform[UMI_ECOSYSTEM_ID_CAPACITY];
    char architecture[UMI_ECOSYSTEM_ID_CAPACITY];
    char toolchain_id[UMI_ECOSYSTEM_ID_CAPACITY];
    uint32_t abi_version;
    UmiEcosystemCompatibility compatibility;
    UmiEcosystemEvidenceState evidence;
    bool installed;
    bool default_sdk;
    bool update_available;
    bool relocation_required;
    uint64_t last_seen_ms;
    uint64_t revision;
} UmiEcosystemSdkEntry;

/**
 * Represent the ecosystem sdk inventory data shared with callers of this public contract.
 */
typedef struct UmiEcosystemSdkInventory {
    UmiEcosystemSdkEntry entries[UMI_ECOSYSTEM_MAX_SDKS];
    size_t entry_count;
    size_t installed_count;
    size_t compatible_count;
    size_t update_count;
    size_t unhealthy_count;
    uint64_t revision;
} UmiEcosystemSdkInventory;

/**
 * Initialise ecosystem sdk entry from caller-provided values so later operations receive a
 * known state.
 */
void umi_ecosystem_sdk_entry_init(
    UmiEcosystemSdkEntry *entry,
    const char *sdk_id,
    const char *display_name,
    const char *version,
    const char *install_root);
/**
 * Initialise ecosystem sdk inventory from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_sdk_inventory_init(UmiEcosystemSdkInventory *inventory);
/**
 * Provide the ecosystem sdk inventory upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_sdk_inventory_upsert(
    UmiEcosystemSdkInventory *inventory,
    const UmiEcosystemSdkEntry *entry);
/**
 * Find ecosystem sdk inventory while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id);
/**
 * Provide the ecosystem sdk inventory find const operation used by this module and its
 * client applications.
 */
const UmiEcosystemSdkEntry *umi_ecosystem_sdk_inventory_find_const(
    const UmiEcosystemSdkInventory *inventory,
    const char *sdk_id);
/**
 * Provide the ecosystem sdk inventory set default operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_sdk_inventory_set_default(
    UmiEcosystemSdkInventory *inventory,
    const char *sdk_id);
/**
 * Provide the ecosystem sdk inventory recalculate operation used by this module and its
 * client applications.
 */
void umi_ecosystem_sdk_inventory_recalculate(
    UmiEcosystemSdkInventory *inventory);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_SDK_INVENTORY_H */
