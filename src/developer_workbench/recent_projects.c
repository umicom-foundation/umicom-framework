/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/recent_projects.c
 *
 * PURPOSE:
 *   Add/touch project entries in the cross-product Framework recent-item store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/recent_projects.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

/*
 * Add developer workbench recent projects only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_workbench_recent_projects_add(
    UmiRecentItemRegistry *registry,
    const char *project_id,
    const char *path,
    const char *label,
    uint64_t opened_at)
{
    UmiRecentItemSnapshot item;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || project_id == NULL || path == NULL ||
        label == NULL || project_id[0] == '\0' || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_platform_recent_items_registry_find(
            registry, project_id, &item) == UMI_STATUS_OK) {
        status = umi_platform_recent_items_registry_touch(
            registry, project_id, opened_at);
        return status;
    }

    (void)memset(&item, 0, sizeof(item));
    item.struct_size = (uint32_t)sizeof(item);
    item.api_version = 1U;
    copy_text(item.id, sizeof(item.id), project_id);
    copy_text(item.uri, sizeof(item.uri), path);
    copy_text(item.label, sizeof(item.label), label);
    copy_text(item.kind, sizeof(item.kind), "project");
    item.last_opened = opened_at;
    item.open_count = 1U;
    item.revision = 1U;

    return umi_platform_recent_items_registry_upsert(registry, &item);
}
