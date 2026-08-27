/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_sdk_inventory.c
 * PURPOSE: Verify SDK inventory health and compatible default selection.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/sdk_inventory.h"

int main(void)
{
    UmiEcosystemSdkInventory inventory;
    UmiEcosystemSdkEntry entry;
    umi_ecosystem_sdk_inventory_init(&inventory);
    umi_ecosystem_sdk_entry_init(&entry, "umicom-1", "Umicom SDK", "1.0.0",
        "C:/umicom/sdk/1.0.0");
    entry.compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    entry.evidence = UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
    assert(umi_ecosystem_sdk_inventory_upsert(&inventory, &entry) ==
        UMI_STATUS_OK);
    assert(inventory.installed_count == 1U && inventory.compatible_count == 1U);
    assert(umi_ecosystem_sdk_inventory_set_default(&inventory, "umicom-1") ==
        UMI_STATUS_OK);
    assert(inventory.entries[0].default_sdk);
    entry.compatibility = UMI_ECOSYSTEM_INCOMPATIBLE;
    assert(umi_ecosystem_sdk_inventory_upsert(&inventory, &entry) ==
        UMI_STATUS_OK);
    assert(inventory.unhealthy_count == 1U);
    assert(!inventory.entries[0].default_sdk);
    assert(umi_ecosystem_sdk_inventory_set_default(&inventory, "umicom-1") ==
        UMI_STATUS_INVALID_STATE);
    return 0;
}
