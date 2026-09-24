/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/start_centre.h
 *
 * PURPOSE:
 *   Build a toolkit-neutral Start Centre snapshot from Framework recent items
 *   and well-known project/workspace commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_START_CENTRE_H
#define UMICOM_DEVELOPER_WORKBENCH_START_CENTRE_H

#include "umicom/platform/recent_items.h"
#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY 32U

/**
 * Represent the developer workbench start centre snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchStartCentreSnapshot {
    UmiRecentItemSnapshot
        recent[UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY];
    size_t recent_count;
    char new_project_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    char open_folder_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    char clone_repository_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    uint64_t revision;
} UmiDeveloperWorkbenchStartCentreSnapshot;

/**
 * Provide the developer workbench start centre snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_start_centre_snapshot(
    const UmiRecentItemRegistry *recent_items,
    UmiDeveloperWorkbenchStartCentreSnapshot *out_snapshot);

/*
 * Project and workspace launchers need to distinguish a valid recent entry
 * from one whose directory has moved or been removed. The registry remains the
 * source of truth; this projection adds only read-only availability evidence.
 */
typedef struct UmiDeveloperWorkbenchRecentWorkItem {
    UmiRecentItemSnapshot recent;
    int available;
} UmiDeveloperWorkbenchRecentWorkItem;

typedef struct UmiDeveloperWorkbenchRecentWorkSnapshot {
    UmiDeveloperWorkbenchRecentWorkItem
        items[UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY];
    size_t count;
    size_t pinned_count;
    size_t unavailable_count;
    uint64_t revision;
} UmiDeveloperWorkbenchRecentWorkSnapshot;

/**
 * Project one recent-work kind for a Start Centre or launcher.
 *
 * kind must be "project" or "workspace". Results preserve the Framework
 * recent-item ordering: pinned entries first, then most recently opened work.
 * A zero limit uses the full bounded Start Centre capacity.
 */
UmiStatus umi_developer_workbench_start_centre_recent_work_snapshot(
    const UmiRecentItemRegistry *recent_items,
    const char *kind,
    size_t limit,
    UmiDeveloperWorkbenchRecentWorkSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
