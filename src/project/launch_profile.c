/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/launch_profile.c
 *
 * PURPOSE:
 *   Implement a reusable project-system record used by Studio and future Umicom development products.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/project/launch_profile.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectLaunchProfileRegistry {
    UmiProjectLaunchProfileSnapshot items[UMI_PROJECT_LAUNCH_PROFILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiProjectLaunchProfileRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_project_launch_profile_registry_create(UmiProjectLaunchProfileRegistry **out_registry)
{
    UmiProjectLaunchProfileRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiProjectLaunchProfileRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_project_launch_profile_registry_destroy(UmiProjectLaunchProfileRegistry *registry) { free(registry); }

UmiStatus umi_project_launch_profile_registry_upsert(UmiProjectLaunchProfileRegistry *registry, const UmiProjectLaunchProfileSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_PROJECT_LAUNCH_PROFILE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_PROJECT_LAUNCH_PROFILE_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].project_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].program[1023U] = '\0';
    registry->items[index].arguments[1023U] = '\0';
    registry->items[index].working_directory[1023U] = '\0';
    registry->items[index].environment_id[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_launch_profile_registry_remove(UmiProjectLaunchProfileRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_project_launch_profile_registry_find(const UmiProjectLaunchProfileRegistry *registry, const char *id, UmiProjectLaunchProfileSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_project_launch_profile_registry_at(const UmiProjectLaunchProfileRegistry *registry, size_t index, UmiProjectLaunchProfileSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_project_launch_profile_registry_count(const UmiProjectLaunchProfileRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_project_launch_profile_registry_revision(const UmiProjectLaunchProfileRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_project_launch_profile_registry_clear(UmiProjectLaunchProfileRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
