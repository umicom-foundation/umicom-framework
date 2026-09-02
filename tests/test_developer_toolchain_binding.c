/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_developer_toolchain_binding.c
 *
 * PURPOSE:
 *   Implement the test developer toolchain binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework universal developer toolchain binding regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT. */
#include <assert.h>
#include <string.h>

#include "umicom/developer/toolchain_binding.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperToolchainBindingRegistry *registry = NULL;
    UmiDeveloperToolchainBindingRegistrySnapshot registry_snapshot;
    UmiDeveloperToolchainBindingSnapshot binding;

    assert(umi_developer_toolchain_binding_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_register_builtins(registry) ==
           UMI_STATUS_OK);
    assert(umi_developer_toolchain_binding_registry_snapshot(
               registry, &registry_snapshot) == UMI_STATUS_OK);
    assert(registry_snapshot.binding_count ==
           umi_developer_toolchain_binding_builtin_count());
    assert(registry_snapshot.binding_count >= 20U);

    assert(umi_developer_toolchain_binding_registry_resolve(
               registry, "c", "windows", "x86_64",
               UMI_LANGUAGE_CAPABILITY_BUILD |
                   UMI_LANGUAGE_CAPABILITY_DEBUG,
               &binding) == UMI_STATUS_OK);
    assert(strcmp(binding.id, "c.msvc") == 0);
    assert(strcmp(binding.compiler, "cl") == 0);

    assert(umi_developer_toolchain_binding_registry_resolve(
               registry, "c", "linux", "riscv64",
               UMI_LANGUAGE_CAPABILITY_MACHINE_CODE, &binding) == UMI_STATUS_OK);
    assert(strcmp(binding.id, "c.clang") == 0);
    assert(umi_developer_toolchain_binding_registry_resolve(
               registry, "python", "windows", "x86_64",
               UMI_LANGUAGE_CAPABILITY_REPL, &binding) == UMI_STATUS_OK);
    assert(strcmp(binding.id, "python.cpython") == 0);
    assert(umi_developer_toolchain_binding_registry_resolve(
               registry, "html", "linux", "x86_64",
               UMI_LANGUAGE_CAPABILITY_DEBUG, &binding) == UMI_STATUS_NOT_FOUND);

    umi_developer_toolchain_binding_registry_destroy(registry);
    return 0;
}
