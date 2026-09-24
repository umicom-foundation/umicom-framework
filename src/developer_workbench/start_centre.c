/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/start_centre.c
 *
 * PURPOSE:
 *   Project recent workspace state into a Start Centre model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/start_centre.h"

#include <stdio.h>
#include <string.h>

#include "umicom/platform/filesystem.h"

/*
 * Provide the developer workbench start centre snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_start_centre_snapshot(
    const UmiRecentItemRegistry *recent_items,
    UmiDeveloperWorkbenchStartCentreSnapshot *out_snapshot)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (recent_items == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->new_project_command,
                   sizeof(out_snapshot->new_project_command),
                   "%s",
                   "project.new");
    (void)snprintf(out_snapshot->open_folder_command,
                   sizeof(out_snapshot->open_folder_command),
                   "%s",
                   "workspace.open-folder");
    (void)snprintf(out_snapshot->clone_repository_command,
                   sizeof(out_snapshot->clone_repository_command),
                   "%s",
                   "repository.clone");

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_platform_recent_items_registry_count(recent_items) &&
         out_snapshot->recent_count <
             UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY;
         ++index) {
        UmiRecentItemSnapshot item;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_platform_recent_items_registry_at(
                recent_items, index, &item) != UMI_STATUS_OK) {
            continue;
        }

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(item.kind, "project") != 0 &&
            strcmp(item.kind, "workspace") != 0) {
            continue;
        }

        out_snapshot->recent[out_snapshot->recent_count++] = item;
    }

    out_snapshot->revision =
        umi_platform_recent_items_registry_revision(recent_items);
    return UMI_STATUS_OK;
}

/*
 * Add availability evidence without changing the existing MRU registry or its
 * sorting rules. This keeps filesystem checks out of the portable recent-item
 * store while allowing a Start Centre to avoid opening a path that disappeared.
 */
UmiStatus umi_developer_workbench_start_centre_recent_work_snapshot(
    const UmiRecentItemRegistry *recent_items,
    const char *kind,
    size_t limit,
    UmiDeveloperWorkbenchRecentWorkSnapshot *out_snapshot)
{
    UmiRecentItemQuery query = {0};
    UmiRecentItemSnapshot recent[
        UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY];
    size_t count = 0U;
    size_t index;
    UmiStatus status;

    if (recent_items == NULL || kind == NULL || kind[0] == '\0' ||
        out_snapshot == NULL ||
        (strcmp(kind, "project") != 0 && strcmp(kind, "workspace") != 0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)memset(recent, 0, sizeof(recent));

    if (limit == 0U ||
        limit > UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY) {
        limit = UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY;
    }

    query.kind = kind;
    query.limit = limit;
    status = umi_platform_recent_items_registry_query(
        recent_items,
        &query,
        recent,
        sizeof(recent) / sizeof(recent[0]),
        &count);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < count; ++index) {
        UmiDeveloperWorkbenchRecentWorkItem *item =
            &out_snapshot->items[out_snapshot->count++];

        item->recent = recent[index];
        item->available = umi_fs_is_directory(item->recent.uri) ? 1 : 0;
        if (item->recent.pinned != 0) out_snapshot->pinned_count += 1U;
        if (item->available == 0) out_snapshot->unavailable_count += 1U;
    }

    out_snapshot->revision =
        umi_platform_recent_items_registry_revision(recent_items);
    return UMI_STATUS_OK;
}
