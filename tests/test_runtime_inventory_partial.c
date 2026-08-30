/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_inventory_partial.c
 *
 * PURPOSE:
 *   Verify that runtime inventory snapshots support partial compositions in
 *   which one or more registry surfaces are intentionally absent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/inventory.h"

#include <assert.h>

int main(void)
{
    UmiRuntimeInventorySnapshot snapshot;

    assert(umi_runtime_inventory_snapshot(
        NULL, NULL, NULL, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.command_count == 0U);
    assert(snapshot.service_count == 0U);
    assert(snapshot.capability_count == 0U);
    assert(snapshot.structure_size == sizeof(snapshot));
    assert(umi_runtime_inventory_snapshot(NULL, NULL, NULL, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
