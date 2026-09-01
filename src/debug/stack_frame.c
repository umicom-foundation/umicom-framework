/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/stack_frame.c
 *
 * PURPOSE:
 *   Implement a DAP-friendly but adapter-neutral debugger record for native and future Umicom runtimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/debug/stack_frame.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugStackFrameRegistry {
    UmiDebugStackFrameSnapshot items[UMI_DEBUG_STACK_FRAME_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiDebugStackFrameRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_debug_stack_frame_registry_create(UmiDebugStackFrameRegistry **out_registry)
{
    UmiDebugStackFrameRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDebugStackFrameRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_debug_stack_frame_registry_destroy(UmiDebugStackFrameRegistry *registry) { free(registry); }

UmiStatus umi_debug_stack_frame_registry_upsert(UmiDebugStackFrameRegistry *registry, const UmiDebugStackFrameSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_DEBUG_STACK_FRAME_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_DEBUG_STACK_FRAME_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].thread_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].source_uri[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_stack_frame_registry_remove(UmiDebugStackFrameRegistry *registry, const char *id)
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

UmiStatus umi_debug_stack_frame_registry_find(const UmiDebugStackFrameRegistry *registry, const char *id, UmiDebugStackFrameSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_debug_stack_frame_registry_at(const UmiDebugStackFrameRegistry *registry, size_t index, UmiDebugStackFrameSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_debug_stack_frame_registry_count(const UmiDebugStackFrameRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_debug_stack_frame_registry_revision(const UmiDebugStackFrameRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_debug_stack_frame_registry_clear(UmiDebugStackFrameRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
