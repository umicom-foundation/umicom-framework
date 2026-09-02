/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/types.c
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
#include "umicom/distribution/runtime/types.h"

#include <string.h>

/* Provide the dr copy text operation used by this module and its client applications. */
UmiStatus umi_dr_copy_text(char *dst, size_t capacity, const char *src) {
    size_t n;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || capacity == 0U || src == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    n = strlen(src);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (n >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(dst, src, n + 1U);
    return UMI_STATUS_OK;
}
/* Provide the dr hash text operation used by this module and its client applications. */
uint64_t umi_dr_hash_text(const char *text) {
    uint64_t h = UINT64_C(1469598103934665603);
    const unsigned char *p = (const unsigned char *)text;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*p != 0U) { h ^= (uint64_t)*p++; h *= UINT64_C(1099511628211); }
    return h;
}
/* Provide the dr hash combine operation used by this module and its client applications. */
uint64_t umi_dr_hash_combine(uint64_t seed, uint64_t value) { return seed ^ (value + UINT64_C(0x9e3779b97f4a7c15) + (seed << 6U) + (seed >> 2U)); }
/*
 * Provide the dr version compare operation used by this module and its client
 * applications.
 */
int umi_dr_version_compare(UmiDrVersion a, UmiDrVersion b) {
    /* Apply this branch only when its contract condition is satisfied. */
    if (a.major != b.major) return a.major < b.major ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a.minor != b.minor) return a.minor < b.minor ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a.patch != b.patch) return a.patch < b.patch ? -1 : 1;
    return 0;
}
/*
 * Provide the dr version at least operation used by this module and its client
 * applications.
 */
bool umi_dr_version_at_least(UmiDrVersion actual, UmiDrVersion minimum) { return umi_dr_version_compare(actual, minimum) >= 0; }
/* Provide the dr platform text operation used by this module and its client applications. */
const char *umi_dr_platform_text(UmiDrPlatform p) { /* Select the behaviour associated with the requested command or state value. */ switch(p){case UMI_DR_PLATFORM_WINDOWS:return "windows";case UMI_DR_PLATFORM_LINUX:return "linux";case UMI_DR_PLATFORM_MACOS:return "macos";case UMI_DR_PLATFORM_FREEBSD:return "freebsd";case UMI_DR_PLATFORM_UMICOM_OS:return "umicom-os";default:return "unknown";} }
/*
 * Provide the dr architecture text operation used by this module and its client
 * applications.
 */
const char *umi_dr_architecture_text(UmiDrArchitecture a) { /* Select the behaviour associated with the requested command or state value. */ switch(a){case UMI_DR_ARCH_X86_64:return "x86_64";case UMI_DR_ARCH_ARM64:return "arm64";case UMI_DR_ARCH_RISCV64:return "riscv64";default:return "unknown";} }
/*
 * Provide the dr package format text operation used by this module and its client
 * applications.
 */
const char *umi_dr_package_format_text(UmiDrPackageFormat f) { /* Select the behaviour associated with the requested command or state value. */ switch(f){case UMI_DR_PACKAGE_ZIP:return "zip";case UMI_DR_PACKAGE_TAR:return "tar";case UMI_DR_PACKAGE_MSI:return "msi";case UMI_DR_PACKAGE_MSIX:return "msix";case UMI_DR_PACKAGE_DEB:return "deb";case UMI_DR_PACKAGE_RPM:return "rpm";case UMI_DR_PACKAGE_APPIMAGE:return "appimage";case UMI_DR_PACKAGE_DMG:return "dmg";case UMI_DR_PACKAGE_PKG:return "pkg";case UMI_DR_PACKAGE_PORTABLE:return "portable";default:return "unknown";} }
/* Provide the dr channel text operation used by this module and its client applications. */
const char *umi_dr_channel_text(UmiDrChannelKind c) { /* Select the behaviour associated with the requested command or state value. */ switch(c){case UMI_DR_CHANNEL_DEVELOPMENT:return "development";case UMI_DR_CHANNEL_NIGHTLY:return "nightly";case UMI_DR_CHANNEL_BETA:return "beta";case UMI_DR_CHANNEL_STABLE:return "stable";case UMI_DR_CHANNEL_LTS:return "lts";default:return "unknown";} }
