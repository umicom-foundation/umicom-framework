/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/output_channel.c
 *
 * PURPOSE:
 *   Implement an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/ui/output_channel.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiUiOutputChannelRegistry {
    UmiUiOutputChannelSnapshot items[UMI_UI_OUTPUT_CHANNEL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiOutputChannelRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_output_channel_registry_create(UmiUiOutputChannelRegistry **out_registry)
{
    UmiUiOutputChannelRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiOutputChannelRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_output_channel_registry_destroy(UmiUiOutputChannelRegistry *registry) { free(registry); }

UmiStatus umi_ui_output_channel_registry_upsert(UmiUiOutputChannelRegistry *registry, const UmiUiOutputChannelSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_OUTPUT_CHANNEL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_UI_OUTPUT_CHANNEL_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].category[127U] = '\0';
    registry->items[index].text[2047U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_output_channel_registry_remove(UmiUiOutputChannelRegistry *registry, const char *id)
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

UmiStatus umi_ui_output_channel_registry_find(const UmiUiOutputChannelRegistry *registry, const char *id, UmiUiOutputChannelSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_ui_output_channel_registry_at(const UmiUiOutputChannelRegistry *registry, size_t index, UmiUiOutputChannelSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_ui_output_channel_registry_count(const UmiUiOutputChannelRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_ui_output_channel_registry_revision(const UmiUiOutputChannelRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_ui_output_channel_registry_clear(UmiUiOutputChannelRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
