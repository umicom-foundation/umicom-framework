/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_contract_abi.c
 *
 * PURPOSE:
 *   Verify the stable C ABI and descriptor-size contract exported to Framework
 *   applications, adapters and plug-ins.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <assert.h>

int main(void)
{
    UmiRuntimeContractSnapshot snapshot;

    assert(umi_runtime_contract_snapshot(&snapshot) == UMI_STATUS_OK);
    assert(snapshot.framework_abi_version == UMICOM_FRAMEWORK_ABI_VERSION);
    assert(snapshot.command_descriptor_size == sizeof(UmiCommandDescriptor));
    assert(snapshot.service_descriptor_size == sizeof(UmiServiceDescriptor));
    assert(snapshot.capability_descriptor_size == sizeof(UmiCapabilityDescriptor));
    assert(snapshot.module_lifecycle_size == sizeof(UmiModuleLifecycle));
    assert(snapshot.module_descriptor_size == sizeof(UmiModuleDescriptor));
    assert(snapshot.inventory_snapshot_size == sizeof(UmiRuntimeInventorySnapshot));
    assert(umi_runtime_contract_abi_matches(UMICOM_FRAMEWORK_ABI_VERSION));
    assert(!umi_runtime_contract_abi_matches(UMICOM_FRAMEWORK_ABI_VERSION + 1U));
    return 0;
}
