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

UmiStatus umi_developer_workbench_start_centre_snapshot(
    const UmiRecentItemRegistry *recent_items,
    UmiDeveloperWorkbenchStartCentreSnapshot *out_snapshot)
{
    size_t index;

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

    for (index = 0U;
         index < umi_platform_recent_items_registry_count(recent_items) &&
         out_snapshot->recent_count <
             UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY;
         ++index) {
        UmiRecentItemSnapshot item;

        if (umi_platform_recent_items_registry_at(
                recent_items, index, &item) != UMI_STATUS_OK) {
            continue;
        }

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
