/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_toolchain_capability.c
 *
 * PURPOSE:
 *   Implement the test toolchain capability behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/toolchain/toolchain.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiToolchainProfile profile;
    UmiToolchainCapabilitySnapshot capability;
    UmiToolInfo *tool;

    /* A stale caller size must be rejected without touching its allocation. */
    (void)memset(&profile, 0xA5, sizeof(profile));
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_profile_initialize(&profile, sizeof(profile) - 1U) !=
            UMI_STATUS_INVALID_ARGUMENT ||
        (unsigned char)profile.profile_id[0] != 0xA5U) {
        return EXIT_FAILURE;
    }

    /* The source-compatible initializer records the current ABI identity. */
    umi_toolchain_profile_init(&profile);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_toolchain_profile_storage_compatible(&profile)) {
        return EXIT_FAILURE;
    }
    strcpy(profile.profile_id, "fixture-gcc");
    profile.family = UMI_TOOLCHAIN_POSIX_GCC;
    profile.selected_c_compiler = UMI_TOOL_GCC;
    profile.selected_cpp_compiler = UMI_TOOL_GXX;
    profile.c23_capable = 1;
    profile.complete = 1;
    strcpy(profile.target_triple, "x86_64-w64-mingw32");
    tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_GCC);
    tool->state = UMI_TOOL_VALIDATED;
    strcpy(tool->version, "gcc (GCC) 16.1.0");
    tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CMAKE);
    tool->state = UMI_TOOL_VALIDATED;
    tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_CTEST);
    tool->state = UMI_TOOL_VALIDATED;
    tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_NINJA);
    tool->state = UMI_TOOL_VALIDATED;
    tool = umi_toolchain_profile_tool_mutable(&profile, UMI_TOOL_PKG_CONFIG);
    tool->state = UMI_TOOL_VALIDATED;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_capability_snapshot(&profile, &capability) !=
        UMI_STATUS_OK) return EXIT_FAILURE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (capability.compiler_vendor != UMI_COMPILER_VENDOR_GCC ||
        capability.compiler_version_major != 16U ||
        strcmp(capability.target_triple, "x86_64-w64-mingw32") != 0 ||
        !capability.c23_available || !capability.cmake_available ||
        !capability.ninja_available || !capability.pkg_config_available ||
        !capability.compilation_database_supported ||
        !umi_toolchain_capability_compatible(&capability, 1, 1, 1, 1))
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
