/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_sbom_license.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/sbom.h"
#include "umicom/delivery/license_inventory.h"
int main(void) {
    UmiSbom sbom;
    UmiLicenseInventory licenses;
    umi_sbom_init(&sbom);
    umi_license_inventory_init(&licenses);
    assert(umi_sbom_add(&sbom, "gtk", "4", "GNOME", "LGPL-2.1") == UMI_STATUS_OK);
    assert(umi_license_inventory_add(&licenses, "gtk", "LGPL-2.1", "licenses/gtk.txt") == UMI_STATUS_OK);
    assert(umi_sbom_find(&sbom, "gtk") != 0);
    assert(umi_license_inventory_contains(&licenses, "gtk"));
    return 0;
}
