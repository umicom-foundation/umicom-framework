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

typedef struct UmiDeveloperWorkbenchStartCentreSnapshot {
    UmiRecentItemSnapshot
        recent[UMI_DEVELOPER_WORKBENCH_START_RECENT_CAPACITY];
    size_t recent_count;
    char new_project_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    char open_folder_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    char clone_repository_command[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    uint64_t revision;
} UmiDeveloperWorkbenchStartCentreSnapshot;

UmiStatus umi_developer_workbench_start_centre_snapshot(
    const UmiRecentItemRegistry *recent_items,
    UmiDeveloperWorkbenchStartCentreSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
