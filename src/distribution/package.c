/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/package.c
 *
 * PURPOSE:
 *   Implement bounded Distribution API v2 package metadata and semantic
 *   version helpers used by repository and policy services.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/package.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static int copy_text(char *destination, size_t capacity, const char *source)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    written = snprintf(destination, capacity, "%s", source);
    return written >= 0 && (size_t)written < capacity;
}

/*
 * Initialise distribution package from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_distribution_package_init(
    UmiDistributionPackage *package,
    const char *release_id,
    const char *package_id,
    const char *product_id,
    const char *display_name,
    UmiVersion version,
    UmiDistributionPackageKind kind,
    UmiReleaseChannel channel)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || release_id == NULL || package_id == NULL ||
        product_id == NULL || display_name == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(package, 0, sizeof(*package));
    package->struct_size = (uint32_t)sizeof(*package);
    package->api_version = UMI_DISTRIBUTION_API_VERSION;
    package->version = version;
    package->kind = kind;
    package->channel = channel;
    package->compatible = 1;
    package->revision = 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!copy_text(package->release_id, sizeof(package->release_id), release_id) ||
        !copy_text(package->package_id, sizeof(package->package_id), package_id) ||
        !copy_text(package->product_id, sizeof(package->product_id), product_id) ||
        !copy_text(package->display_name, sizeof(package->display_name), display_name)) {
        (void)memset(package, 0, sizeof(*package));
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/*
 * Check that distribution package satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_distribution_package_validate(
    const UmiDistributionPackage *package,
    char *out_reason,
    size_t reason_capacity)
{
    const char *reason = "valid";
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package == NULL || out_reason == NULL || reason_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (package->struct_size < sizeof(*package) ||
        package->api_version != UMI_DISTRIBUTION_API_VERSION) {
        reason = "unsupported distribution package contract";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (package->release_id[0] == '\0' || package->package_id[0] == '\0' ||
               package->product_id[0] == '\0' || package->display_name[0] == '\0') {
        reason = "missing release, package, product or display identity";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (package->version.major == 0U && package->version.minor == 0U &&
               package->version.patch == 0U) {
        reason = "version 0.0.0 is not distributable";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (package->kind < UMI_DISTRIBUTION_APPLICATION ||
               package->kind > UMI_DISTRIBUTION_CONTENT) {
        reason = "unknown package kind";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (package->channel < UMI_RELEASE_DEVELOPMENT ||
               package->channel > UMI_RELEASE_STABLE) {
        reason = "unknown release channel";
        status = UMI_STATUS_INVALID_STATE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (package->required_framework_abi == 0U) {
        reason = "missing required Framework ABI";
        status = UMI_STATUS_INVALID_STATE;
    }
    (void)snprintf(out_reason, reason_capacity, "%s", reason);
    return status;
}

/*
 * Provide the distribution version compare operation used by this module and its client
 * applications.
 */
int umi_distribution_version_compare(UmiVersion left, UmiVersion right)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.major != right.major) return left.major < right.major ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.minor != right.minor) return left.minor < right.minor ? -1 : 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left.patch != right.patch) return left.patch < right.patch ? -1 : 1;
    return 0;
}

/*
 * Provide the distribution package kind text operation used by this module and its client
 * applications.
 */
const char *umi_distribution_package_kind_text(UmiDistributionPackageKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DISTRIBUTION_APPLICATION: return "application";
        case UMI_DISTRIBUTION_EXTENSION: return "extension";
        case UMI_DISTRIBUTION_RUNTIME: return "runtime";
        case UMI_DISTRIBUTION_TEMPLATE: return "template";
        case UMI_DISTRIBUTION_CONTENT: return "content";
        default: return "unknown";
    }
}
