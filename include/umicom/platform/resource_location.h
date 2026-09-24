/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_location.h
 *
 * PURPOSE:
 *   Define normalised local and remote resource locations and the per-user
 *   application directories used by Umicom products.  Applications can keep
 *   settings, recovery files, caches and other writable state away from their
 *   installation folder and away from whichever directory happened to launch
 *   the process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This contract stores bounded snapshots by value. The registry owns those
 * copies; it does not take ownership of strings or external resources.
 * Coordinate cross-thread mutation at the product/service boundary.
 */
#ifndef UMICOM_PLATFORM_RESOURCE_LOCATION_H
#define UMICOM_PLATFORM_RESOURCE_LOCATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY 1024U
#define UMI_APPLICATION_PATHS_API_VERSION 1U

/**
 * Represent the resource location snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiResourceLocationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char id[128];
    char uri[1024];
    char display_name[256];
    char scheme[64];
    char authority[256];
    char path[1024];
    int local;
    int writable;
    int available;
    uint64_t revision;
} UmiResourceLocationSnapshot;

/**
 * Represent the resource location registry data shared with callers of this public
 * contract.
 */
typedef struct UmiResourceLocationRegistry UmiResourceLocationRegistry;

/**
 * Initialise platform resource location registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry);
/**
 * Release or reset state held by platform resource location registry so the same storage
 * can be reused safely.
 */
void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry);
/**
 * Provide the platform resource location registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item);
/**
 * Remove platform resource location registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id);
/**
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item);
/**
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item);
/**
 * Return the number of records represented by platform resource location registry without
 * changing their state.
 */
size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry);
/**
 * Provide the platform resource location registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry);

/*
 * Writable application paths are an additive Framework capability.  The
 * resource-location API above is intentionally left unchanged so existing
 * applications retain their established public contract.
 */
#define UMI_APPLICATION_PATHS_API_VERSION 1U

/**
 * Input used to resolve writable application directories.
 *
 * baseOverride is intended for isolated tests, portable developer runs and
 * controlled hosts.  When it is NULL, Framework uses the operating system's
 * normal per-user application-data locations.  A supplied override must be an
 * absolute path so the result can never depend on the process working
 * directory.
 */
typedef struct UmiApplicationPathsConfig {
    uint32_t structSize;
    uint32_t apiVersion;
    const char *organisationDirectory;
    const char *applicationDirectory;
    const char *baseOverride;
} UmiApplicationPathsConfig;

/**
 * Writable directories owned by one Umicom application.
 *
 * On Windows these directories live below the current user's LOCALAPPDATA
 * folder.  On Unix-like systems Framework follows the XDG configuration,
 * data, state and cache locations when they are available.  No member is
 * resolved from the current working directory.
 */
typedef struct UmiApplicationPaths {
    uint32_t structSize;
    uint32_t apiVersion;
    char root[UMI_PATH_CAPACITY];
    char config[UMI_PATH_CAPACITY];
    char state[UMI_PATH_CAPACITY];
    char cache[UMI_PATH_CAPACITY];
    char data[UMI_PATH_CAPACITY];
    char logs[UMI_PATH_CAPACITY];
    char recovery[UMI_PATH_CAPACITY];
} UmiApplicationPaths;

/**
 * Create a default application-path request for an Umicom product.
 *
 * applicationDirectory is a directory name such as "Studio" or "Trader".
 * The returned configuration uses "Umicom" as the organisation directory and
 * leaves baseOverride unset.
 */
UmiApplicationPathsConfig UmiApplicationPathsConfigDefault(
    const char *applicationDirectory);

/**
 * Resolve the current user's writable locations without creating directories.
 */
UmiStatus UmiApplicationPathsResolve(
    const UmiApplicationPathsConfig *config,
    UmiApplicationPaths *outPaths);

/**
 * Create the resolved writable directories when they do not already exist.
 */
UmiStatus UmiApplicationPathsPrepare(
    const UmiApplicationPaths *paths);

#ifdef __cplusplus
}
#endif

#endif
