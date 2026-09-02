/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_location.h
 *
 * PURPOSE:
 *   Define normalised local and remote resource locations without binding callers to a GUI.
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

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY 1024U

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

#ifdef __cplusplus
}
#endif

#endif
