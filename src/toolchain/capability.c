/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/capability.c
 * PURPOSE: Convert validated profiles into stable capability snapshots.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/capability.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int tool_ready(const UmiToolchainProfile *profile, UmiToolKind kind)
{
    const UmiToolInfo *tool = umi_toolchain_profile_tool(profile, kind);
    return tool != NULL && tool->state == UMI_TOOL_VALIDATED;
}

static void parse_version(const char *text, uint32_t *major,
                          uint32_t *minor, uint32_t *patch)
{
    const char *cursor = text != NULL ? text : "";
    unsigned first = 0U;
    unsigned second = 0U;
    unsigned third = 0U;
    while (*cursor != '\0' && !isdigit((unsigned char)*cursor)) cursor += 1;
    if (sscanf(cursor, "%u.%u.%u", &first, &second, &third) < 1) return;
    *major = (uint32_t)first;
    *minor = (uint32_t)second;
    *patch = (uint32_t)third;
}

static UmiCompilerVendor vendor_from_kind(UmiToolKind kind)
{
    if (kind == UMI_TOOL_CLANG) return UMI_COMPILER_VENDOR_CLANG;
    if (kind == UMI_TOOL_GCC) return UMI_COMPILER_VENDOR_GCC;
    if (kind == UMI_TOOL_MSVC_CL) return UMI_COMPILER_VENDOR_MSVC;
    return UMI_COMPILER_VENDOR_UNKNOWN;
}

UmiStatus umi_toolchain_capability_snapshot(
    const UmiToolchainProfile *profile,
    UmiToolchainCapabilitySnapshot *out_snapshot)
{
    const UmiToolInfo *compiler;
    if (profile == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TOOLCHAIN_CAPABILITY_API_VERSION;
    (void)snprintf(out_snapshot->profile_id,
                   sizeof(out_snapshot->profile_id), "%s", profile->profile_id);
    (void)snprintf(out_snapshot->target_triple,
                   sizeof(out_snapshot->target_triple), "%s",
                   profile->target_triple);
    out_snapshot->compiler_kind = profile->selected_c_compiler;
    out_snapshot->compiler_vendor = vendor_from_kind(
        profile->selected_c_compiler);
    compiler = umi_toolchain_profile_c_compiler(profile);
    if (compiler != NULL && compiler->state == UMI_TOOL_VALIDATED) {
        out_snapshot->compiler_available = 1;
        parse_version(compiler->version,
                      &out_snapshot->compiler_version_major,
                      &out_snapshot->compiler_version_minor,
                      &out_snapshot->compiler_version_patch);
    }
    out_snapshot->c23_available = profile->c23_capable;
    out_snapshot->cmake_available = tool_ready(profile, UMI_TOOL_CMAKE);
    out_snapshot->ctest_available = tool_ready(profile, UMI_TOOL_CTEST);
    out_snapshot->ninja_available = tool_ready(profile, UMI_TOOL_NINJA);
    out_snapshot->pkg_config_available = tool_ready(profile, UMI_TOOL_PKG_CONFIG);
    out_snapshot->debugger_available = tool_ready(profile, UMI_TOOL_GDB);
    out_snapshot->resource_compiler_available =
        tool_ready(profile, UMI_TOOL_WINDRES) ||
        profile->family == UMI_TOOLCHAIN_MSVC;
    out_snapshot->compilation_database_supported =
        out_snapshot->cmake_available && out_snapshot->ninja_available;
    out_snapshot->environment_is_complete = profile->complete;
    return UMI_STATUS_OK;
}

int umi_toolchain_capability_compatible(
    const UmiToolchainCapabilitySnapshot *snapshot,
    int require_c23,
    int require_cmake,
    int require_ninja,
    int require_pkg_config)
{
    if (snapshot == NULL || !snapshot->compiler_available) return 0;
    if (require_c23 && !snapshot->c23_available) return 0;
    if (require_cmake && !snapshot->cmake_available) return 0;
    if (require_ninja && !snapshot->ninja_available) return 0;
    if (require_pkg_config && !snapshot->pkg_config_available) return 0;
    return 1;
}

const char *umi_compiler_vendor_text(UmiCompilerVendor vendor)
{
    switch (vendor) {
        case UMI_COMPILER_VENDOR_CLANG: return "Clang";
        case UMI_COMPILER_VENDOR_GCC: return "GCC";
        case UMI_COMPILER_VENDOR_MSVC: return "MSVC";
        default: return "Unknown";
    }
}
