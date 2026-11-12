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

/*
 * Initialise toolchain profile from caller-provided values so later operations receive a
 * known state.
 */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_TOOL_COUNT; ++index) {
        profile->tools[index].kind = (UmiToolKind)index;
        profile->tools[index].state = UMI_TOOL_MISSING;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain profile storage compatible operation used by this module and its
 * client applications.
 */
bool umi_toolchain_profile_storage_compatible(
    const UmiToolchainProfile *profile)
{
    /* Validate the self-describing prefix before any later field is read. */
    return profile != NULL &&
           profile->structure_size == sizeof(*profile) &&
           profile->api_version == UMI_TOOLCHAIN_PROFILE_API_VERSION &&
           profile->tool_count <= UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY;
}

/*
 * Provide the toolchain profile tool operation used by this module and its client
 * applications.
 */
/* Only expose initialized slots within the profile's declared tool count.
 * The previous enum-only boundary is retained for engineering review. */
#if 0
const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT
        ? &profile->tools[(size_t)kind]
        : NULL;
}
#endif
const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT && (size_t)kind < profile->tool_count
        ? &profile->tools[(size_t)kind]
        : NULL;
}

/*
 * Provide the toolchain profile tool mutable operation used by this module and its client
 * applications.
 */
/* Only expose initialized slots within the profile's declared tool count.
 * The previous enum-only boundary is retained for engineering review. */
#if 0
UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT
        ? &profile->tools[(size_t)kind]
        : NULL;
}
#endif
UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               kind >= 0 && kind < UMI_TOOL_COUNT && (size_t)kind < profile->tool_count
        ? &profile->tools[(size_t)kind]
        : NULL;
}

/*
 * Provide the toolchain family text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_family_text(UmiToolchainFamily family)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the toolchain profile c compiler operation used by this module and its client
 * applications.
 */
const UmiToolInfo *umi_toolchain_profile_c_compiler(
    const UmiToolchainProfile *profile)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               profile->selected_c_compiler < UMI_TOOL_COUNT
        ? umi_toolchain_profile_tool(profile, profile->selected_c_compiler)
        : NULL;
}

/*
 * Provide the toolchain profile cpp compiler operation used by this module and its client
 * applications.
 */
const UmiToolInfo *umi_toolchain_profile_cpp_compiler(
    const UmiToolchainProfile *profile)
{
    return umi_toolchain_profile_storage_compatible(profile) &&
               profile->selected_cpp_compiler < UMI_TOOL_COUNT
        ? umi_toolchain_profile_tool(profile, profile->selected_cpp_compiler)
        : NULL;
}


bool umi_toolchain_profile_storage_valid(const UmiToolchainProfile *profile)
{
    if (!umi_toolchain_profile_storage_compatible(profile) ||
        !memchr(profile->profile_id,'\0',sizeof(profile->profile_id)) ||
        !memchr(profile->display_name,'\0',sizeof(profile->display_name)) ||
        !memchr(profile->root,'\0',sizeof(profile->root)) ||
        !memchr(profile->bin_directory,'\0',sizeof(profile->bin_directory)) ||
        !memchr(profile->prefix_directory,'\0',sizeof(profile->prefix_directory)) ||
        !memchr(profile->generator,'\0',sizeof(profile->generator)) ||
        !memchr(profile->target_triple,'\0',sizeof(profile->target_triple))) return false;
    /* Reserved slots keep the public ABI independent of the current tool enum.
     * Inspect only declared known slots; consumers still admit the selected kind. */
    for (size_t index = 0U; index < profile->tool_count && index < (size_t)UMI_TOOL_COUNT; ++index) {
        const UmiToolInfo *tool = &profile->tools[index];
        if (tool->kind != (UmiToolKind)index ||
            tool->state < UMI_TOOL_MISSING || tool->state > UMI_TOOL_FAILED ||
            !memchr(tool->path,'\0',sizeof(tool->path)) ||
            !memchr(tool->version,'\0',sizeof(tool->version))) return false;
    }
    return true;
}
