/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/profile.c
 *
 * PURPOSE:
 *   Initialise and inspect validated native toolchain profiles shared by the
 *   Umicom command, Studio, generated applications, and future suite managers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/profile.h"

#include <string.h>

void umi_toolchain_profile_init(UmiToolchainProfile *profile)
{
    size_t index;
    if (profile == NULL) {
        return;
    }
    (void)memset(profile, 0, sizeof(*profile));
    profile->tool_count = UMI_TOOL_COUNT;
    for (index = 0U; index < UMI_TOOL_COUNT; ++index) {
        profile->tools[index].kind = (UmiToolKind)index;
        profile->tools[index].state = UMI_TOOL_MISSING;
    }
}

const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return profile != NULL && kind >= 0 && kind < UMI_TOOL_COUNT
        ? &profile->tools[(size_t)kind]
        : NULL;
}

UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind)
{
    return profile != NULL && kind >= 0 && kind < UMI_TOOL_COUNT
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
        default: return "Unknown";
    }
}
