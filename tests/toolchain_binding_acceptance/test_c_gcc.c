/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_c_gcc.c
 *
 * PURPOSE:
 *   Verify the existing c.gcc developer toolchain binding remains registered and reports its current readiness accurately.
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

/*
 * Exercise test toolchain binding acceptance c gcc and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_c_gcc(void)
{
    UmiDeveloperToolchainBindingRegistry *registry = NULL;
    UmiDeveloperToolchainBindingSnapshot binding;
    UmiDeveloperToolchainReadiness readiness;

    assert(umi_test_toolchain_binding_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_find(
        registry,
        "c.gcc",
        &binding) == UMI_STATUS_OK);

    assert(strcmp(binding.language_id, "c") == 0);
    assert((binding.capabilities & UMI_LANGUAGE_CAPABILITY_BUILD) == UMI_LANGUAGE_CAPABILITY_BUILD);
    assert(umi_developer_toolchain_binding_readiness(
        &binding,
        umi_test_all_programs_available,
        NULL,
        &readiness) == UMI_STATUS_OK);

    assert(strcmp(readiness.binding_id, "c.gcc") == 0);
    assert(readiness.supported_operation_count > 0U);
    assert(readiness.missing_operation_count == 0U);

    umi_developer_toolchain_binding_registry_destroy(registry);
    return 0;
}
