/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_inventory_environment.c
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
#include "umicom/delivery/inventory.h"
#include "umicom/delivery/environment.h"
int main(void) {
    UmiDeliveryInventory inventory;
    UmiDeliveryEnvironment environment;
    umi_delivery_inventory_init(&inventory);
    assert(umi_delivery_inventory_add(&inventory, "bin/app", 10U) == UMI_STATUS_OK);
    assert(umi_delivery_inventory_total_bytes(&inventory) == 10U);
    assert(umi_delivery_environment_init(&environment, "windows", "x86_64", "release") == UMI_STATUS_OK);
    return 0;
}
