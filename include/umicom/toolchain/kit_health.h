/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/kit_health.h
 * PURPOSE: Evaluate developer-kit readiness and actionable missing capability.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TOOLCHAIN_KIT_HEALTH_H
#define UMICOM_TOOLCHAIN_KIT_HEALTH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/toolchain/kit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_KIT_HEALTH_API_VERSION 1U

typedef uint64_t UmiToolchainKitHealthIssueFlags;
enum {
    UMI_TOOLCHAIN_KIT_HEALTH_DISABLED = UINT64_C(1) << 0,
    UMI_TOOLCHAIN_KIT_HEALTH_UNAVAILABLE = UINT64_C(1) << 1,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_COMPILER = UINT64_C(1) << 2,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_BUILD_SYSTEM = UINT64_C(1) << 3,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TEST_RUNNER = UINT64_C(1) << 4,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_DEBUGGER = UINT64_C(1) << 5,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SDK = UINT64_C(1) << 6,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_TARGET = UINT64_C(1) << 7,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_SYSROOT = UINT64_C(1) << 8,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_C23 = UINT64_C(1) << 9,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_PYTHON = UINT64_C(1) << 10,
    UMI_TOOLCHAIN_KIT_HEALTH_MISSING_JAVA = UINT64_C(1) << 11,
    UMI_TOOLCHAIN_KIT_HEALTH_CAPABILITY_GAP = UINT64_C(1) << 12
};

typedef enum UmiToolchainKitHealthState {
    UMI_TOOLCHAIN_KIT_HEALTH_UNKNOWN = 0,
    UMI_TOOLCHAIN_KIT_HEALTH_HEALTHY = 1,
    UMI_TOOLCHAIN_KIT_HEALTH_DEGRADED = 2,
    UMI_TOOLCHAIN_KIT_HEALTH_UNHEALTHY = 3
} UmiToolchainKitHealthState;

typedef struct UmiToolchainKitHealthSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiToolchainKitHealthState state;
    UmiToolchainKitHealthIssueFlags issue_flags;
    UmiToolchainKitCapabilityFlags required_capabilities;
    UmiToolchainKitCapabilityFlags supported_capabilities;
    UmiToolchainKitCapabilityFlags missing_capabilities;
    size_t issue_count;
    int ready;
} UmiToolchainKitHealthSnapshot;

UmiStatus umi_toolchain_kit_health_evaluate(
    const UmiToolchainKitSnapshot *kit,
    UmiToolchainKitCapabilityFlags required_capabilities,
    int require_sysroot_for_cross_compile,
    UmiToolchainKitHealthSnapshot *out_health);
const char *umi_toolchain_kit_health_state_text(
    UmiToolchainKitHealthState state);

#ifdef __cplusplus
}
#endif
#endif
