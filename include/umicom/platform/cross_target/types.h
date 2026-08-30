/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/types.h
 *
 * PURPOSE:
 *   Define common cross-target identifiers, limits, support levels, health states and shared utilities.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_TYPES_H
#define UMICOM_PLATFORM_CROSS_TARGET_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CT_API_VERSION 1U
#define UMI_CT_ID_CAPACITY 128U
#define UMI_CT_NAME_CAPACITY 128U
#define UMI_CT_TRIPLE_CAPACITY 160U
#define UMI_CT_PATH_CAPACITY 512U
#define UMI_CT_MAX_ITEMS 64U
#define UMI_CT_MAX_DEPENDENCIES 32U
#define UMI_CT_MAX_FEATURES 64U

typedef enum UmiCtArchitecture {
    UMI_CT_ARCH_UNKNOWN = 0,
    UMI_CT_ARCH_X86_64 = 1,
    UMI_CT_ARCH_ARM64 = 2,
    UMI_CT_ARCH_RISCV32 = 3,
    UMI_CT_ARCH_RISCV64 = 4
} UmiCtArchitecture;

typedef enum UmiCtOperatingSystem {
    UMI_CT_OS_UNKNOWN = 0,
    UMI_CT_OS_WINDOWS = 1,
    UMI_CT_OS_LINUX = 2,
    UMI_CT_OS_MACOS = 3,
    UMI_CT_OS_FREEBSD = 4,
    UMI_CT_OS_UMICOM = 5,
    UMI_CT_OS_BARE_METAL = 6
} UmiCtOperatingSystem;

typedef enum UmiCtEnvironment {
    UMI_CT_ENV_UNKNOWN = 0,
    UMI_CT_ENV_GNU = 1,
    UMI_CT_ENV_MSVC = 2,
    UMI_CT_ENV_MUSL = 3,
    UMI_CT_ENV_NONE = 4,
    UMI_CT_ENV_UMICOM = 5
} UmiCtEnvironment;

typedef enum UmiCtEndian { UMI_CT_ENDIAN_LITTLE = 1, UMI_CT_ENDIAN_BIG = 2 } UmiCtEndian;
typedef enum UmiCtSupportLevel { UMI_CT_SUPPORT_NONE = 0, UMI_CT_SUPPORT_DEGRADED = 1, UMI_CT_SUPPORT_NATIVE = 2 } UmiCtSupportLevel;
typedef enum UmiCtHealth { UMI_CT_HEALTH_UNKNOWN = 0, UMI_CT_HEALTH_READY = 1, UMI_CT_HEALTH_DEGRADED = 2, UMI_CT_HEALTH_BLOCKED = 3 } UmiCtHealth;

typedef struct UmiCtTarget {
    uint32_t structure_size;
    uint32_t api_version;
    char triple[UMI_CT_TRIPLE_CAPACITY];
    char vendor[UMI_CT_NAME_CAPACITY];
    UmiCtArchitecture architecture;
    UmiCtOperatingSystem operating_system;
    UmiCtEnvironment environment;
    uint32_t pointer_bits;
    UmiCtEndian endian;
} UmiCtTarget;

bool umi_ct_id_valid(const char *text);
UmiStatus umi_ct_copy(char *destination, size_t capacity, const char *source);
uint64_t umi_ct_hash_bytes(uint64_t seed, const void *data, size_t size);
uint64_t umi_ct_hash_text(uint64_t seed, const char *text);
const char *umi_ct_support_level_text(UmiCtSupportLevel level);
const char *umi_ct_health_text(UmiCtHealth health);

#ifdef __cplusplus
}
#endif

#endif
