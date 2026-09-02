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

/**
 * Represent the developer project workbench platform data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperProjectWorkbenchPlatform
    UmiDeveloperProjectWorkbenchPlatform;

/**
 * Represent the developer project workbench platform snapshot data shared with callers of
 * this public contract.
 */
typedef struct UmiDeveloperProjectWorkbenchPlatformSnapshot {
    UmiDeveloperWorkbenchSnapshot workbench;
    UmiDeveloperProjectServiceSnapshot projects;
    int new_project_ready;
    char selected_application_preset[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint64_t revision;
} UmiDeveloperProjectWorkbenchPlatformSnapshot;

/**
 * Initialise developer project workbench platform from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_project_workbench_platform_create(
    const UmiDeveloperWorkbenchBindings *bindings,
    UmiDeveloperProjectWorkbenchPlatform **out_platform);

/**
 * Release or reset state held by developer project workbench platform so the same storage
 * can be reused safely.
 */
void umi_developer_project_workbench_platform_destroy(
    UmiDeveloperProjectWorkbenchPlatform *platform);

/**
 * Provide the developer project workbench platform workbench operation used by this module
 * and its client applications.
 */
UmiDeveloperWorkbench *
umi_developer_project_workbench_platform_workbench(
    UmiDeveloperProjectWorkbenchPlatform *platform);

/**
 * Provide the developer project workbench platform projects operation used by this module
 * and its client applications.
 */
UmiDeveloperProjectService *
umi_developer_project_workbench_platform_projects(
    UmiDeveloperProjectWorkbenchPlatform *platform);

/**
 * Provide the developer project workbench platform wizard operation used by this module
 * and its client applications.
 */
UmiDeveloperWorkbenchProjectWizard *
umi_developer_project_workbench_platform_wizard(
    UmiDeveloperProjectWorkbenchPlatform *platform);

/**
 * Provide the developer project workbench platform snapshot operation used by this module
 * and its client applications.
 */
UmiStatus umi_developer_project_workbench_platform_snapshot(
    UmiDeveloperProjectWorkbenchPlatform *platform,
    UmiDeveloperProjectWorkbenchPlatformSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
