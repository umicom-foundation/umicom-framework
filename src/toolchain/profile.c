/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/profile.c
 *
 * PURPOSE:
 *   Initialise and inspect validated native toolchain profiles shared by the
 *   Umicom command, Studio, generated applications, and future suite managers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/profile.h"

#include <stdio.h>
#include <string.h>

UmiStatus umi_toolchain_profile_initialize(
    UmiToolchainProfile *profile,
    size_t caller_structure_size)
{
    size_t index;

    /* Reject an old caller before writing any byte outside its allocation. */
    if (profile == NULL || caller_structure_size != sizeof(*profile)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Establish deterministic defaults for every current and reserved slot. */
    (void)memset(profile, 0, sizeof(*profile));
    profile->structure_size = (uint32_t)sizeof(*profile);
    profile->api_version = UMI_TOOLCHAIN_PROFILE_API_VERSION;
    profile->tool_count = UMI_TOOL_COUNT;
    profile->selected_c_compiler = UMI_TOOL_COUNT;
    profile->selected_cpp_compiler = UMI_TOOL_COUNT;
    (void)snprintf(profile->generator, sizeof(profile->generator), "%s", "Ninja");
    for (index = 0U; index < UMI_TOOL_COUNT; ++index) {
        profile->tools[index].kind = (UmiToolKind)index;
        profile->tools[index].state = UMI_TOOL_MISSING;
    }
    return UMI_STATUS_OK;
}

bool umi_toolchain_profile_storage_compatible(
    const UmiToolchainProfile *profile)
{
    /* Validate the self-describing prefix before any later field is read. */
    return profile != NULL &&
           profile->structure_size == sizeof(*profile) &&
           profile->api_version == UMI_TOOLCHAIN_PROFILE_API_VERSION &&
           profile->tool_count <= UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY;
}

const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT
        ? &profile->tools[(size_t)kind]
        : NULL;
}

UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT
        ? &profile->tools[(size_t)kind]
        : NULL;
}

const char *umi_toolchain_family_text(UmiToolchainFamily family)
{
    switch (family) {
        case UMI_TOOLCHAIN_MSYS2_UCRT64: return "MSYS2 UCRT64";
        case UMI_TOOLCHAIN_MSYS2_CLANG64: return "MSYS2 CLANG64";
        case UMI_TOOLCHAIN_MSVC: return "Microsoft Visual C++";
        case UMI_TOOLCHAIN_POSIX_CLANG: return "POSIX Clang";
        case UMI_TOOLCHAIN_POSIX_GCC: return "POSIX GCC";
        case UMI_TOOLCHAIN_MSYS2_MINGW64: return "MSYS2 MINGW64";
        default: return "Unknown";
    }
}

const UmiToolInfo *umi_toolchain_profile_c_compiler(
    const UmiToolchainProfile *profile)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               profile->selected_c_compiler < UMI_TOOL_COUNT
        ? umi_toolchain_profile_tool(profile, profile->selected_c_compiler)
        : NULL;
}

const UmiToolInfo *umi_toolchain_profile_cpp_compiler(
    const UmiToolchainProfile *profile)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               profile->selected_cpp_compiler < UMI_TOOL_COUNT
        ? umi_toolchain_profile_tool(profile, profile->selected_cpp_compiler)
        : NULL;
}
