/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit.h
 *
 * PURPOSE:
 *   Describe one complete developer kit: compiler profile, SDK, target, sysroot,
 *   native tools and runtime dependencies selected as one reusable unit.
 *
 * ARCHITECTURE:
 *   A kit is Framework-owned state. Studio, Desk, CLI and generated products
 *   select kits but do not own compiler discovery or environment construction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_H
#define UMICOM_TOOLCHAIN_KIT_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/toolchain/cross_toolchain.h"
#include "umicom/toolchain/profile.h"
#include "umicom/toolchain/python_runtime.h"
#include "umicom/toolchain/sdk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_API_VERSION 1U
#define UMI_TOOLCHAIN_KIT_ID_CAPACITY 128U
#define UMI_TOOLCHAIN_KIT_NAME_CAPACITY 256U
#define UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY 128U
#define UMI_TOOLCHAIN_KIT_GENERATOR_CAPACITY 64U
#define UMI_TOOLCHAIN_KIT_ENVIRONMENT_CAPACITY 128U

typedef uint64_t UmiToolchainKitCapabilityFlags;
enum {
    UMI_TOOLCHAIN_KIT_CAPABILITY_CONFIGURE = UINT64_C(1) << 0,
    UMI_TOOLCHAIN_KIT_CAPABILITY_BUILD = UINT64_C(1) << 1,
    UMI_TOOLCHAIN_KIT_CAPABILITY_TEST = UINT64_C(1) << 2,
    UMI_TOOLCHAIN_KIT_CAPABILITY_RUN = UINT64_C(1) << 3,
    UMI_TOOLCHAIN_KIT_CAPABILITY_DEBUG = UINT64_C(1) << 4,
    UMI_TOOLCHAIN_KIT_CAPABILITY_PACKAGE = UINT64_C(1) << 5,
    UMI_TOOLCHAIN_KIT_CAPABILITY_C23 = UINT64_C(1) << 6,
    UMI_TOOLCHAIN_KIT_CAPABILITY_CPP = UINT64_C(1) << 7,
    UMI_TOOLCHAIN_KIT_CAPABILITY_ASSEMBLY = UINT64_C(1) << 8,
    UMI_TOOLCHAIN_KIT_CAPABILITY_CROSS_COMPILE = UINT64_C(1) << 9,
    UMI_TOOLCHAIN_KIT_CAPABILITY_SDK = UINT64_C(1) << 10,
    UMI_TOOLCHAIN_KIT_CAPABILITY_PYTHON = UINT64_C(1) << 11,
    UMI_TOOLCHAIN_KIT_CAPABILITY_JAVA = UINT64_C(1) << 12,
    UMI_TOOLCHAIN_KIT_CAPABILITY_GIT = UINT64_C(1) << 13
};

/**
 * List the named toolchain kit state values accepted by this public contract.
 */
typedef enum UmiToolchainKitState {
    UMI_TOOLCHAIN_KIT_UNKNOWN = 0,
    UMI_TOOLCHAIN_KIT_READY = 1,
    UMI_TOOLCHAIN_KIT_DEGRADED = 2,
    UMI_TOOLCHAIN_KIT_UNAVAILABLE = 3
} UmiToolchainKitState;

/**
 * Represent the toolchain kit snapshot data shared with callers of this public contract.
 */
typedef struct UmiToolchainKitSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_TOOLCHAIN_KIT_ID_CAPACITY];
    char display_name[UMI_TOOLCHAIN_KIT_NAME_CAPACITY];
    char profile_id[UMI_TOOLCHAIN_TEXT_CAPACITY];
    char sdk_id[128];
    char host_triple[UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY];
    char target_triple[UMI_TOOLCHAIN_KIT_TRIPLE_CAPACITY];
    char sysroot[UMI_TOOL_PATH_CAPACITY];
    char cmake_generator[UMI_TOOLCHAIN_KIT_GENERATOR_CAPACITY];
    char cmake_toolchain_file[UMI_TOOL_PATH_CAPACITY];
    char environment_profile[UMI_TOOLCHAIN_KIT_ENVIRONMENT_CAPACITY];
    char c_compiler[UMI_TOOL_PATH_CAPACITY];
    char cpp_compiler[UMI_TOOL_PATH_CAPACITY];
    char assembler[UMI_TOOL_PATH_CAPACITY];
    char linker[UMI_TOOL_PATH_CAPACITY];
    char debugger[UMI_TOOL_PATH_CAPACITY];
    char cmake[UMI_TOOL_PATH_CAPACITY];
    char ctest[UMI_TOOL_PATH_CAPACITY];
    char ninja[UMI_TOOL_PATH_CAPACITY];
    char git[UMI_TOOL_PATH_CAPACITY];
    char python[UMI_TOOL_PATH_CAPACITY];
    char java[UMI_TOOL_PATH_CAPACITY];
    char javac[UMI_TOOL_PATH_CAPACITY];
    UmiToolchainKitCapabilityFlags capabilities;
    uint32_t priority;
    UmiToolchainKitState state;
    int enabled;
    uint64_t revision;
} UmiToolchainKitSnapshot;

/**
 * Initialise toolchain kit from caller-provided values so later operations receive a known
 * state.
 */
void umi_toolchain_kit_init(UmiToolchainKitSnapshot *kit,
                            const char *kit_id,
                            const char *display_name);
/**
 * Check that toolchain kit satisfies its contract before another service relies on it.
 */
UmiStatus umi_toolchain_kit_validate(const UmiToolchainKitSnapshot *kit);
/**
 * Provide the toolchain kit from profile operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_from_profile(
    UmiToolchainKitSnapshot *kit,
    const UmiToolchainProfile *profile,
    const UmiSdkSnapshot *sdk,
    const char *host_triple);
/**
 * Provide the toolchain kit apply cross toolchain operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_apply_cross_toolchain(
    UmiToolchainKitSnapshot *kit,
    const UmiCrossToolchainSnapshot *cross_toolchain);
/**
 * Provide the toolchain kit apply python runtime operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_apply_python_runtime(
    UmiToolchainKitSnapshot *kit,
    const UmiPythonRuntimeSnapshot *runtime);
/**
 * Provide the toolchain kit set sysroot operation used by this module and its client
 * applications.
 */
UmiStatus umi_toolchain_kit_set_sysroot(UmiToolchainKitSnapshot *kit,
                                       const char *sysroot);
/**
 * Provide the toolchain kit set cmake toolchain file operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_set_cmake_toolchain_file(
    UmiToolchainKitSnapshot *kit,
    const char *path);
/**
 * Provide the toolchain kit set environment profile operation used by this module and its
 * client applications.
 */
UmiStatus umi_toolchain_kit_set_environment_profile(
    UmiToolchainKitSnapshot *kit,
    const char *profile_id);
/**
 * Provide the toolchain kit is cross compile operation used by this module and its client
 * applications.
 */
int umi_toolchain_kit_is_cross_compile(const UmiToolchainKitSnapshot *kit);
/**
 * Provide the toolchain kit supports operation used by this module and its client
 * applications.
 */
int umi_toolchain_kit_supports(const UmiToolchainKitSnapshot *kit,
                               UmiToolchainKitCapabilityFlags required);
/**
 * Provide the toolchain kit state text operation used by this module and its client
 * applications.
 */
const char *umi_toolchain_kit_state_text(UmiToolchainKitState state);

#ifdef __cplusplus
}
#endif
#endif
