/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/action_binding.c
 *
 * PURPOSE:
 *   Implement visual-designer action bindings to canonical Framework commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/designer/action_binding.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerActionBindingRegistry {
    UmiDesignerActionBindingSnapshot items[UMI_DESIGNER_ACTION_BINDING_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiDesignerActionBindingRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_designer_action_binding_registry_create(UmiDesignerActionBindingRegistry **out_registry)
{
    UmiDesignerActionBindingRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDesignerActionBindingRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_designer_action_binding_registry_destroy(UmiDesignerActionBindingRegistry *registry)
{
    free(registry);
}

UmiStatus umi_designer_action_binding_registry_upsert(UmiDesignerActionBindingRegistry *registry, const UmiDesignerActionBindingSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_DESIGNER_ACTION_BINDING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiDesignerActionBindingSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_action_binding_registry_remove(UmiDesignerActionBindingRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_action_binding_registry_find(const UmiDesignerActionBindingRegistry *registry, const char *id, UmiDesignerActionBindingSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_action_binding_registry_at(const UmiDesignerActionBindingRegistry *registry, size_t index, UmiDesignerActionBindingSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_designer_action_binding_registry_count(const UmiDesignerActionBindingRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_designer_action_binding_registry_revision(const UmiDesignerActionBindingRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
