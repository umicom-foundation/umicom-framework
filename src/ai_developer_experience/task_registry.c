/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_registry.c
 *
 * PURPOSE:
 *   Implement bounded task-summary storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperTaskRegistry {
    UmiAiDeveloperTaskEntry items[UMI_AI_DEVELOPER_TASK_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id)
{
    size_t index;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].task_id, task_id) == 0) {
            return index;
        }
    }

    return registry->count;
}

UmiStatus umi_ai_developer_task_registry_create(
    UmiAiDeveloperTaskRegistry **out_registry)
{
    UmiAiDeveloperTaskRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiAiDeveloperTaskRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ai_developer_task_registry_destroy(
    UmiAiDeveloperTaskRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ai_developer_task_registry_upsert(
    UmiAiDeveloperTaskRegistry *registry,
    const UmiAiDeveloperTaskEntry *entry)
{
    size_t index;

    if (registry == NULL || entry == NULL ||
        entry->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, entry->task_id);

    if (index == registry->count) {
        if (registry->count >= UMI_AI_DEVELOPER_TASK_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        registry->count += 1U;
    }

    registry->items[index] = *entry;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_task_registry_find(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id,
    UmiAiDeveloperTaskEntry *out_entry)
{
    size_t index;

    if (registry == NULL || task_id == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, task_id);
    if (index == registry->count) return UMI_STATUS_NOT_FOUND;

    *out_entry = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_developer_task_registry_at(
    const UmiAiDeveloperTaskRegistry *registry,
    size_t index,
    UmiAiDeveloperTaskEntry *out_entry)
{
    if (registry == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_entry = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ai_developer_task_registry_count(
    const UmiAiDeveloperTaskRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ai_developer_task_registry_revision(
    const UmiAiDeveloperTaskRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

void umi_ai_developer_task_registry_clear(
    UmiAiDeveloperTaskRegistry *registry)
{
    if (registry == NULL) return;

    (void)memset(registry->items, 0, sizeof(registry->items));
    registry->count = 0U;
    registry->revision += 1U;
}
