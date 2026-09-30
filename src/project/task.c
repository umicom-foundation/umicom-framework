/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/project/task.c
 *
 * PURPOSE:
 *   Implement a reusable project-system record used by Studio and future Umicom development products.
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
#include "umicom/project/task.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, project_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, label, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, command, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, working_directory, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiProjectTaskSnapshot, group, 0)
};

UmiStatus umi_project_task_snapshot_validate(const UmiProjectTaskSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiProjectTaskRegistry {
    UmiProjectTaskSnapshot items[UMI_PROJECT_TASK_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiProjectTaskRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise project task registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_project_task_registry_create(UmiProjectTaskRegistry **out_registry)
{
    UmiProjectTaskRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiProjectTaskRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by project task registry so the same storage can be reused
 * safely.
 */
void umi_project_task_registry_destroy(UmiProjectTaskRegistry *registry) { free(registry); }

/*
 * Provide the project task registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_project_task_registry_upsert(UmiProjectTaskRegistry *registry, const UmiProjectTaskSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_project_task_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry != NULL && registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_PROJECT_TASK_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_PROJECT_TASK_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].project_id[127U] = '\0';
    registry->items[index].label[255U] = '\0';
    registry->items[index].command[1023U] = '\0';
    registry->items[index].working_directory[1023U] = '\0';
    registry->items[index].group[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove project task registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_project_task_registry_remove(UmiProjectTaskRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find project task registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_project_task_registry_find(const UmiProjectTaskRegistry *registry, const char *id, UmiProjectTaskSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Find project task registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_project_task_registry_at(const UmiProjectTaskRegistry *registry, size_t index, UmiProjectTaskSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by project task registry without changing their
 * state.
 */
size_t umi_project_task_registry_count(const UmiProjectTaskRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the project task registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_project_task_registry_revision(const UmiProjectTaskRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by project task registry so the same storage can be reused
 * safely.
 */
void umi_project_task_registry_clear(UmiProjectTaskRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}

/* Stage a complete value-only registry before publishing a batch. Existing
 * upsert semantics run against the private copy, so any failed row leaves the
 * caller's count, records and revision unchanged. No application duplicate is
 * needed, and the original single-record implementation remains available. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_project_task_registry_upsert_many,
    UmiProjectTaskRegistry, UmiProjectTaskSnapshot,
    umi_project_task_snapshot_validate, umi_project_task_registry_upsert, UMI_PROJECT_TASK_CAPACITY)
