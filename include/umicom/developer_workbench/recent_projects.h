/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/recent_projects.h
 *
 * PURPOSE:
 *   Store project/workspace recent-item evidence through the existing Framework
 *   MRU registry instead of creating a Studio-private recent-project database.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_RECENT_PROJECTS_H
#define UMICOM_DEVELOPER_WORKBENCH_RECENT_PROJECTS_H

#include "umicom/platform/recent_items.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_developer_workbench_recent_projects_add(
    UmiRecentItemRegistry *registry,
    const char *project_id,
    const char *path,
    const char *label,
    uint64_t opened_at);

#ifdef __cplusplus
}
#endif

#endif
