/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_registry.c
 *
 * PURPOSE:
 *   Implement bounded task-summary storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiDeveloperTaskRegistry {
    UmiAiDeveloperTaskEntry items[UMI_AI_DEVELOPER_TASK_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id)
{
    size_t index;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].task_id, task_id) == 0) {
            return index;
        }
    }

    return registry->count;
}

/*
 * Initialise ai developer task registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_task_registry_create(
    UmiAiDeveloperTaskRegistry **out_registry)
{
    UmiAiDeveloperTaskRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiAiDeveloperTaskRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai developer task registry so the same storage can be
 * reused safely.
 */
void umi_ai_developer_task_registry_destroy(
    UmiAiDeveloperTaskRegistry *registry)
{
    free(registry);
}

/*
 * Provide the ai developer task registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_registry_upsert(
    UmiAiDeveloperTaskRegistry *registry,
    const UmiAiDeveloperTaskEntry *entry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || entry == NULL ||
        entry->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, entry->task_id);

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == registry->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_AI_DEVELOPER_TASK_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        registry->count += 1U;
    }

    registry->items[index] = *entry;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ai developer task registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_developer_task_registry_find(
    const UmiAiDeveloperTaskRegistry *registry,
    const char *task_id,
    UmiAiDeveloperTaskEntry *out_entry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || task_id == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, task_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == registry->count) return UMI_STATUS_NOT_FOUND;

    *out_entry = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find ai developer task registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_developer_task_registry_at(
    const UmiAiDeveloperTaskRegistry *registry,
    size_t index,
    UmiAiDeveloperTaskEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_entry = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai developer task registry without changing
 * their state.
 */
size_t umi_ai_developer_task_registry_count(
    const UmiAiDeveloperTaskRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the ai developer task registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ai_developer_task_registry_revision(
    const UmiAiDeveloperTaskRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/*
 * Release or reset state held by ai developer task registry so the same storage can be
 * reused safely.
 */
void umi_ai_developer_task_registry_clear(
    UmiAiDeveloperTaskRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;

    (void)memset(registry->items, 0, sizeof(registry->items));
    registry->count = 0U;
    registry->revision += 1U;
}
