/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/license_inventory.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_LICENSE_INVENTORY_H
#define INCLUDE_UMICOM_DELIVERY_LICENSE_INVENTORY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLicenseEntry {
    char component[UMI_DELIVERY_ID_CAPACITY];
    char license_id[UMI_DELIVERY_ID_CAPACITY];
    char notice_path[UMI_DELIVERY_PATH_CAPACITY];
} UmiLicenseEntry;

typedef struct UmiLicenseInventory {
    UmiLicenseEntry entries[UMI_DELIVERY_MAX_LICENSES];
    size_t count;
} UmiLicenseInventory;

void umi_license_inventory_init(UmiLicenseInventory *inventory);
UmiStatus umi_license_inventory_add(UmiLicenseInventory *inventory,
                                    const char *component,
                                    const char *license_id,
                                    const char *notice_path);
int umi_license_inventory_contains(const UmiLicenseInventory *inventory,
                                   const char *component);

#ifdef __cplusplus
}
#endif

#endif
