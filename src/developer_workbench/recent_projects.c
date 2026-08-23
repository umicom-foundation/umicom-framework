/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/recent_projects.c
 *
 * PURPOSE:
 *   Add/touch project entries in the cross-product Framework recent-item store.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/recent_projects.h"

#include <string.h>

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";

    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

UmiStatus umi_developer_workbench_recent_projects_add(
    UmiRecentItemRegistry *registry,
    const char *project_id,
    const char *path,
    const char *label,
    uint64_t opened_at)
{
    UmiRecentItemSnapshot item;
    UmiStatus status;

    if (registry == NULL || project_id == NULL || path == NULL ||
        label == NULL || project_id[0] == '\0' || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

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
