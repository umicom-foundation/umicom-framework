/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/resource_location.h
 *
 * PURPOSE:
 *   Define normalised local and remote resource locations without binding callers to a GUI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

typedef struct UmiResourceLocationRegistry UmiResourceLocationRegistry;

UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry);
void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry);
UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item);
UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id);
UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item);
UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item);
size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry);
uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
