/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/profile.h
 *
 * PURPOSE:
 *   Describe one validated native development environment without changing the
 *   parent shell or global operating-system environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_PROFILE_H
#define UMICOM_TOOLCHAIN_PROFILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_TEXT_CAPACITY 256U
#define UMI_TOOLCHAIN_PROFILE_API_VERSION 2U
/*
 * Keep public profile storage independent from the number of known tool kinds.
 * New tools may be appended to UmiToolKind without silently changing the ABI of
 * every application that embeds a profile.
 */
#define UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY 32U

#if defined(__cplusplus)
static_assert((size_t)UMI_TOOL_COUNT <= UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY,
              "Increase the toolchain profile capacity before adding tools");
#else
_Static_assert((size_t)UMI_TOOL_COUNT <= UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY,
               "Increase the toolchain profile capacity before adding tools");
#endif

/**
 * List the named toolchain family values accepted by this public contract.
 */
typedef enum UmiToolchainFamily {
    UMI_TOOLCHAIN_UNKNOWN = 0,
    UMI_TOOLCHAIN_MSYS2_UCRT64 = 1,
    UMI_TOOLCHAIN_MSYS2_CLANG64 = 2,
    UMI_TOOLCHAIN_MSVC = 3,
    UMI_TOOLCHAIN_POSIX_CLANG = 4,
    UMI_TOOLCHAIN_POSIX_GCC = 5,
    UMI_TOOLCHAIN_MSYS2_MINGW64 = 6
} UmiToolchainFamily;

/**
 * Represent the toolchain profile data shared with callers of this public contract.
 */
typedef struct UmiToolchainProfile {
    uint32_t structure_size;
    uint32_t api_version;
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char display_name[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char root[UMI_TOOL_PATH_CAPACITY];
    char bin_directory[UMI_TOOL_PATH_CAPACITY];
    char prefix_directory[UMI_TOOL_PATH_CAPACITY];
    UmiToolchainFamily family;
    UmiToolInfo tools[UMI_TOOLCHAIN_PROFILE_TOOL_CAPACITY];
    size_t tool_count;
    int complete;
    /* Deterministic compiler and generator selection. These fields are
     * appended for source compatibility with the established v1 profile. */
    UmiToolKind selected_c_compiler;
    UmiToolKind selected_cpp_compiler;
    char generator[64];
    char target_triple[128];
    int c23_capable;
} UmiToolchainProfile;

/*
 * Initialise only when caller and Framework agree on the public record size.
 * The source-compatible macro passes the caller's compile-time size and turns
 * stale incremental objects into a safe validation/link failure instead of a
 * buffer overwrite.
 */
UmiStatus umi_toolchain_profile_initialize(
    UmiToolchainProfile *profile,
    size_t caller_structure_size);
#define umi_toolchain_profile_init(profile)                                  \
    ((void)umi_toolchain_profile_initialize(                                 \
        (profile), sizeof(*(profile))))

/**
 * Provide the toolchain profile storage compatible operation used by this module and its
 * client applications.
 */
bool umi_toolchain_profile_storage_compatible(
    const UmiToolchainProfile *profile);
/**
 * Provide the toolchain profile tool operation used by this module and its client
 * applications.
 */
const UmiToolInfo *umi_toolchain_profile_tool(
    const UmiToolchainProfile *profile,
    UmiToolKind kind
);
/**
 * Provide the toolchain profile tool mutable operation used by this module and its client
 * applications.
 */
UmiToolInfo *umi_toolchain_profile_tool_mutable(
    UmiToolchainProfile *profile,
    UmiToolKind kind
);
/**
 * Provide the toolchain family text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_family_text(UmiToolchainFamily family);
/**
 * Provide the toolchain profile c compiler operation used by this module and its client
 * applications.
 */
const UmiToolInfo *umi_toolchain_profile_c_compiler(
    const UmiToolchainProfile *profile);
/**
 * Provide the toolchain profile cpp compiler operation used by this module and its client
 * applications.
 */
const UmiToolInfo *umi_toolchain_profile_cpp_compiler(
    const UmiToolchainProfile *profile);

#ifdef __cplusplus
}
#endif

#endif
