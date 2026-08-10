/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/profile.h
 *
 * PURPOSE:
 *   Describe one validated native development environment without changing the
 *   parent shell or global operating-system environment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_PROFILE_H
#define UMICOM_TOOLCHAIN_PROFILE_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_TEXT_CAPACITY 256U

typedef enum UmiToolchainFamily {
    UMI_TOOLCHAIN_UNKNOWN = 0,
    UMI_TOOLCHAIN_MSYS2_UCRT64 = 1,
    UMI_TOOLCHAIN_MSYS2_CLANG64 = 2,
    UMI_TOOLCHAIN_MSVC = 3,
    UMI_TOOLCHAIN_POSIX_CLANG = 4,
    UMI_TOOLCHAIN_POSIX_GCC = 5
} UmiToolchainFamily;

typedef struct UmiToolchainProfile {
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char display_name[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char root[UMI_TOOL_PATH_CAPACITY];
    char bin_directory[UMI_TOOL_PATH_CAPACITY];
    char prefix_directory[UMI_TOOL_PATH_CAPACITY];
    UmiToolchainFamily family;
    UmiToolInfo tools[UMI_TOOL_COUNT];
    size_t tool_count;
    int complete;
} UmiToolchainProfile;

void umi_toolchain_profile_init(UmiToolchainProfile *profile);
const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind
);
UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind
);
const char *umi_toolchain_family_text(UmiToolchainFamily family);

#ifdef __cplusplus
}
#endif

#endif
