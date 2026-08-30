/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/project_workbench_platform.h
 *
 * PURPOSE:
 *   Compose the professional Developer Workbench with the canonical project,
 *   language and generator platform, including real New Project command actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_WORKBENCH_PLATFORM_H
#define UMICOM_DEVELOPER_PROJECT_WORKBENCH_PLATFORM_H

#include "umicom/developer_project/workbench_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectWorkbenchPlatform
    UmiDeveloperProjectWorkbenchPlatform;

typedef struct UmiDeveloperProjectWorkbenchPlatformSnapshot {
    UmiDeveloperWorkbenchSnapshot workbench;
    UmiDeveloperProjectServiceSnapshot projects;
    int new_project_ready;
    char selected_application_preset[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperProjectWorkbenchPlatformSnapshot;

UmiStatus umi_developer_project_workbench_platform_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperProjectWorkbenchPlatform **out_platform);

void umi_developer_project_workbench_platform_destroy(
    UmiDeveloperProjectWorkbenchPlatform *platform);

UmiDeveloperWorkbench *
umi_developer_project_workbench_platform_workbench(
    UmiDeveloperProjectWorkbenchPlatform *platform);

UmiDeveloperProjectService *
umi_developer_project_workbench_platform_projects(
    UmiDeveloperProjectWorkbenchPlatform *platform);

UmiDeveloperWorkbenchProjectWizard *
umi_developer_project_workbench_platform_wizard(
    UmiDeveloperProjectWorkbenchPlatform *platform);

UmiStatus umi_developer_project_workbench_platform_snapshot(
    UmiDeveloperProjectWorkbenchPlatform *platform,
    UmiDeveloperProjectWorkbenchPlatformSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
