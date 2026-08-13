/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/capability.h
 *
 * PURPOSE:
 *   Publish a stable, reusable capability snapshot for a discovered compiler
 *   and its build environment. Studio and future Umicom applications consume
 *   this contract instead of parsing vendor version strings independently.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_CAPABILITY_H
#define UMICOM_TOOLCHAIN_CAPABILITY_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_CAPABILITY_API_VERSION 1U

typedef enum UmiCompilerVendor {
    UMI_COMPILER_VENDOR_UNKNOWN = 0,
    UMI_COMPILER_VENDOR_CLANG = 1,
    UMI_COMPILER_VENDOR_GCC = 2,
    UMI_COMPILER_VENDOR_MSVC = 3
} UmiCompilerVendor;

typedef struct UmiToolchainCapabilitySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char target_triple[128];
    UmiCompilerVendor compiler_vendor;
    UmiToolKind compiler_kind;
    uint32_t compiler_version_major;
    uint32_t compiler_version_minor;
    uint32_t compiler_version_patch;
    int compiler_available;
    int c23_available;
    int cmake_available;
    int ctest_available;
    int ninja_available;
    int pkg_config_available;
    int debugger_available;
    int resource_compiler_available;
    int compilation_database_supported;
    int environment_is_complete;
} UmiToolchainCapabilitySnapshot;

UmiStatus umi_toolchain_capability_snapshot(
    const UmiToolchainProfile *profile,
    UmiToolchainCapabilitySnapshot *out_snapshot);
int umi_toolchain_capability_compatible(
    const UmiToolchainCapabilitySnapshot *snapshot,
    int require_c23,
    int require_cmake,
    int require_ninja,
    int require_pkg_config);
const char *umi_compiler_vendor_text(UmiCompilerVendor vendor);

#ifdef __cplusplus
}
#endif
#endif
