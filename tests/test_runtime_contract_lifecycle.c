/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_contract_lifecycle.c
 *
 * PURPOSE:
 *   Verify balanced module lifecycle validation for reusable Master/Slave,
 *   worker, agent, adapter and plug-in descriptors.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <assert.h>
#include <string.h>

static UmiStatus phase(UmiModuleContext *context)
{
    (void)context;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiModuleDescriptor descriptor;
    UmiModuleContractReport report;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    descriptor.module_id = "org.umicom.tests.contract";
    descriptor.display_name = "Contract Test";
    descriptor.module_version = (UmiVersion){1U, 0U, 0U};
    descriptor.kind = UMI_MODULE_SERVICE;
    descriptor.lifecycle.start = phase;
    descriptor.lifecycle.stop = phase;

    assert(umi_runtime_module_contract_validate(&descriptor, &report) ==
           UMI_STATUS_OK);
    assert(report.structure_compatible);
    assert(report.abi_compatible);
    assert(report.identity_valid);
    assert(report.kind_valid);
    assert(report.lifecycle_balanced);

    descriptor.lifecycle.stop = NULL;
    assert(umi_runtime_module_contract_validate(&descriptor, &report) ==
           UMI_STATUS_INVALID_STATE);
    assert(!report.lifecycle_balanced);
    return 0;
}
