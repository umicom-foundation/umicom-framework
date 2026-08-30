/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/types.h
 *
 * PURPOSE:
 *   shared cross-platform distribution vocabulary, stable identifiers, version comparison and deterministic fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_TYPES_H
#define UMICOM_DISTRIBUTION_RUNTIME_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DR_ID_CAPACITY 128U
#define UMI_DR_TEXT_CAPACITY 256U
#define UMI_DR_PATH_CAPACITY 512U
#define UMI_DR_DIGEST_CAPACITY 128U
#define UMI_DR_MAX_ITEMS 128U
#define UMI_DR_MAX_ENV 64U
#define UMI_DR_MAX_DEPENDENCIES 128U

typedef enum UmiDrPlatform { UMI_DR_PLATFORM_WINDOWS=1, UMI_DR_PLATFORM_LINUX=2, UMI_DR_PLATFORM_MACOS=3, UMI_DR_PLATFORM_FREEBSD=4, UMI_DR_PLATFORM_UMICOM_OS=5 } UmiDrPlatform;
typedef enum UmiDrArchitecture { UMI_DR_ARCH_X86_64=1, UMI_DR_ARCH_ARM64=2, UMI_DR_ARCH_RISCV64=3 } UmiDrArchitecture;
typedef enum UmiDrEndian { UMI_DR_ENDIAN_LITTLE=1, UMI_DR_ENDIAN_BIG=2 } UmiDrEndian;
typedef enum UmiDrPackageFormat { UMI_DR_PACKAGE_ZIP=1, UMI_DR_PACKAGE_TAR=2, UMI_DR_PACKAGE_MSI=3, UMI_DR_PACKAGE_MSIX=4, UMI_DR_PACKAGE_DEB=5, UMI_DR_PACKAGE_RPM=6, UMI_DR_PACKAGE_APPIMAGE=7, UMI_DR_PACKAGE_DMG=8, UMI_DR_PACKAGE_PKG=9, UMI_DR_PACKAGE_PORTABLE=10 } UmiDrPackageFormat;
typedef enum UmiDrChannelKind { UMI_DR_CHANNEL_DEVELOPMENT=1, UMI_DR_CHANNEL_NIGHTLY=2, UMI_DR_CHANNEL_BETA=3, UMI_DR_CHANNEL_STABLE=4, UMI_DR_CHANNEL_LTS=5 } UmiDrChannelKind;
typedef enum UmiDrInstallScope { UMI_DR_SCOPE_USER=1, UMI_DR_SCOPE_SYSTEM=2, UMI_DR_SCOPE_PORTABLE=3 } UmiDrInstallScope;
typedef enum UmiDrState { UMI_DR_STATE_UNKNOWN=0, UMI_DR_STATE_PLANNED=1, UMI_DR_STATE_READY=2, UMI_DR_STATE_ACTIVE=3, UMI_DR_STATE_COMMITTED=4, UMI_DR_STATE_ROLLED_BACK=5, UMI_DR_STATE_FAILED=6 } UmiDrState;

typedef struct UmiDrVersion { uint32_t major; uint32_t minor; uint32_t patch; } UmiDrVersion;

UmiStatus umi_dr_copy_text(char *dst, size_t capacity, const char *src);
uint64_t umi_dr_hash_text(const char *text);
uint64_t umi_dr_hash_combine(uint64_t seed, uint64_t value);
int umi_dr_version_compare(UmiDrVersion left, UmiDrVersion right);
bool umi_dr_version_at_least(UmiDrVersion actual, UmiDrVersion minimum);
const char *umi_dr_platform_text(UmiDrPlatform platform);
const char *umi_dr_architecture_text(UmiDrArchitecture architecture);
const char *umi_dr_package_format_text(UmiDrPackageFormat format);
const char *umi_dr_channel_text(UmiDrChannelKind channel);

#ifdef __cplusplus
}
#endif
#endif
