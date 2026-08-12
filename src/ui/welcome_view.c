/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/welcome_view.c
 *
 * PURPOSE:
 *   Implement contextual welcome content for empty or not-yet-configured views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/ui/welcome_view.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiWelcomeItemRegistry {
    UmiUiWelcomeItemSnapshot items[UMI_UI_WELCOME_VIEW_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiWelcomeItemRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_welcome_view_registry_create(UmiUiWelcomeItemRegistry **out_registry)
{
    UmiUiWelcomeItemRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiWelcomeItemRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_welcome_view_registry_destroy(UmiUiWelcomeItemRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ui_welcome_view_registry_upsert(UmiUiWelcomeItemRegistry *registry, const UmiUiWelcomeItemSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_WELCOME_VIEW_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiWelcomeItemSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_welcome_view_registry_remove(UmiUiWelcomeItemRegistry *registry, const char *id)
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

UmiStatus umi_ui_welcome_view_registry_find(const UmiUiWelcomeItemRegistry *registry, const char *id, UmiUiWelcomeItemSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_welcome_view_registry_at(const UmiUiWelcomeItemRegistry *registry, size_t index, UmiUiWelcomeItemSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ui_welcome_view_registry_count(const UmiUiWelcomeItemRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ui_welcome_view_registry_revision(const UmiUiWelcomeItemRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
