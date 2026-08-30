/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_assembly_llvm.c
 *
 * PURPOSE:
 *   Verify the existing assembly.llvm developer toolchain binding remains registered and reports its current readiness accurately.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_support.h"

int test_toolchain_binding_acceptance_assembly_llvm(void)
{
    UmiDeveloperToolchainBindingRegistry *registry = NULL;
    UmiDeveloperToolchainBindingSnapshot binding;
    UmiDeveloperToolchainReadiness readiness;

    assert(umi_test_toolchain_binding_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_find(
        registry,
        "assembly.llvm",
        &binding) == UMI_STATUS_OK);

    assert(strcmp(binding.language_id, "assembly") == 0);
    assert((binding.capabilities & UMI_LANGUAGE_CAPABILITY_BUILD) == UMI_LANGUAGE_CAPABILITY_BUILD);
    assert(umi_developer_toolchain_binding_readiness(
        &binding,
        umi_test_all_programs_available,
        NULL,
        &readiness) == UMI_STATUS_OK);

    assert(strcmp(readiness.binding_id, "assembly.llvm") == 0);
    assert(readiness.supported_operation_count > 0U);
    assert(readiness.missing_operation_count == 1U);

    umi_developer_toolchain_binding_registry_destroy(registry);
    return 0;
}
