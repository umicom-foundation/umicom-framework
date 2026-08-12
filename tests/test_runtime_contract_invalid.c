/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime_contract_invalid.c
 *
 * PURPOSE:
 *   Verify deterministic diagnostics for malformed module descriptors instead
 *   of allowing incompatible runtime contracts to proceed silently.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/contract.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiModuleDescriptor descriptor;
    UmiModuleContractReport report;

    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)(sizeof(descriptor) - 1U);
    descriptor.abi_version = UMICOM_FRAMEWORK_ABI_VERSION + 1U;
    descriptor.module_id = "";
    descriptor.display_name = NULL;
    descriptor.kind = (UmiModuleKind)99;

    assert(umi_runtime_module_contract_validate(&descriptor, &report) ==
           UMI_STATUS_INVALID_STATE);
    assert(!report.structure_compatible);
    assert(!report.abi_compatible);
    assert(!report.identity_valid);
    assert(!report.kind_valid);
    assert(report.lifecycle_balanced);
    assert(umi_runtime_module_contract_validate(NULL, &report) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_runtime_module_contract_validate(&descriptor, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
