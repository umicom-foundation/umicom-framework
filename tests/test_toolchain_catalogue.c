/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_catalogue.c
 *
 * PURPOSE:
 *   Implement the test toolchain catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - Batch 35 multi-profile catalogue regression test.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/toolchain/toolchain.h"

/*
 * Exercise make profile and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void make_profile(UmiToolchainProfile *profile, const char *id,
                         UmiToolKind compiler, int c23)
{
    umi_toolchain_profile_init(profile);
    strcpy(profile->profile_id, id);
    profile->selected_c_compiler = compiler;
    profile->selected_cpp_compiler = compiler == UMI_TOOL_GCC
        ? UMI_TOOL_GXX : UMI_TOOL_CLANGXX;
    profile->complete = 1;
    profile->c23_capable = c23;
    umi_toolchain_profile_tool_mutable(profile, compiler)->state =
        UMI_TOOL_VALIDATED;
    umi_toolchain_profile_tool_mutable(profile, UMI_TOOL_CMAKE)->state =
        UMI_TOOL_VALIDATED;
    umi_toolchain_profile_tool_mutable(profile, UMI_TOOL_NINJA)->state =
        UMI_TOOL_VALIDATED;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainCatalogue *catalogue = NULL;
    UmiToolchainProfile clang_profile;
    UmiToolchainProfile gcc_profile;
    UmiToolchainProfile resolved;
    UmiToolchainProjectBindingSnapshot binding = {0};
    UmiToolchainCatalogueSnapshot snapshot;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_toolchain_catalogue_create(&catalogue) != UMI_STATUS_OK) return 1;
    make_profile(&clang_profile, "clang", UMI_TOOL_CLANG, 1);
    make_profile(&gcc_profile, "gcc", UMI_TOOL_GCC, 1);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_catalogue_upsert_profile(catalogue, &gcc_profile) !=
            UMI_STATUS_OK ||
        umi_toolchain_catalogue_upsert_profile(catalogue, &clang_profile) !=
            UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_catalogue_select_profile(catalogue, "clang") !=
        UMI_STATUS_OK) return 3;
    binding.struct_size = (uint32_t)sizeof(binding);
    binding.api_version = UMI_TOOLCHAIN_CATALOGUE_API_VERSION;
    strcpy(binding.project_id, "framework");
    strcpy(binding.profile_id, "gcc");
    strcpy(binding.compilation_database, "/build/compile_commands.json");
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_catalogue_bind_project(catalogue, &binding) !=
        UMI_STATUS_OK) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_catalogue_resolve_project_profile(
            catalogue, "framework", 1, &resolved) != UMI_STATUS_OK ||
        strcmp(resolved.profile_id, "gcc") != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_catalogue_resolve_project_profile(
            catalogue, "studio", 1, &resolved) != UMI_STATUS_OK ||
        strcmp(resolved.profile_id, "clang") != 0) return 6;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_toolchain_catalogue_snapshot(catalogue, &snapshot) != UMI_STATUS_OK ||
        snapshot.profile_count != 2U || snapshot.c23_profile_count != 2U ||
        snapshot.project_binding_count != 1U || !snapshot.has_selected_profile)
        return 7;
    umi_toolchain_catalogue_destroy(catalogue);
    return EXIT_SUCCESS;
}
