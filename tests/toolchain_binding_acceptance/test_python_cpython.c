/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain_binding_acceptance/test_python_cpython.c
 *
 * PURPOSE:
 *   Verify the existing python.cpython developer toolchain binding remains registered and reports its current readiness accurately.
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
 * Exercise test toolchain binding acceptance python cpython and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_toolchain_binding_acceptance_python_cpython(void)
{
    UmiDeveloperToolchainBindingRegistry *registry = NULL;
    UmiDeveloperToolchainBindingSnapshot binding;
    UmiDeveloperToolchainReadiness readiness;

    assert(umi_test_toolchain_binding_registry_create(
        &registry) == UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_find(
        registry,
        "python.cpython",
        &binding) == UMI_STATUS_OK);

    assert(strcmp(binding.language_id, "python") == 0);
    assert((binding.capabilities & UMI_LANGUAGE_CAPABILITY_RUN) == UMI_LANGUAGE_CAPABILITY_RUN);
    assert(umi_developer_toolchain_binding_readiness(
        &binding,
        umi_test_all_programs_available,
        NULL,
        &readiness) == UMI_STATUS_OK);

    assert(strcmp(readiness.binding_id, "python.cpython") == 0);
    assert(readiness.supported_operation_count > 0U);
    assert(readiness.missing_operation_count == 0U);

    umi_developer_toolchain_binding_registry_destroy(registry);
    return 0;
}
