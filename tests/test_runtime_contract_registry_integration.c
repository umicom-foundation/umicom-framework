/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_contract_registry_integration.c
 *
 * PURPOSE:
 *   Verify that runtime contract metadata and registry inventory can be used
 *   together as one application-level compatibility and diagnostics boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <assert.h>

int main(void)
{
    UmiRuntimeContractSnapshot contract;
    UmiRuntimeInventorySnapshot inventory;

    assert(umi_runtime_contract_snapshot(&contract) == UMI_STATUS_OK);
    assert(umi_runtime_inventory_snapshot(NULL, NULL, NULL, &inventory) ==
           UMI_STATUS_OK);
    assert(contract.inventory_snapshot_size == sizeof(inventory));
    assert(contract.framework_abi_version == UMICOM_FRAMEWORK_ABI_VERSION);
    assert(inventory.api_version == UMI_RUNTIME_INVENTORY_API_VERSION);
    return 0;
}
