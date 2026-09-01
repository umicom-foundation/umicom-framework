/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/project_workflow.h
 *
 * PURPOSE:
 *   Resolve project defaults and build a standard configure/build/test/run/debug
 *   workflow directly from reusable project metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Project records describe what can be built and run; the developer runtime
 * describes how operations are scheduled and observed. This bridge keeps those
 * two layers separate while giving applications one safe high-level command.
 */
#ifndef UMICOM_DEVELOPER_PROJECT_WORKFLOW_H
#define UMICOM_DEVELOPER_PROJECT_WORKFLOW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/developer/workflow.h"
#include "umicom/project/workspace_query.h"
#include "umicom/project/workspace_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_PROJECT_WORKFLOW_API_VERSION 1U

typedef enum UmiDeveloperProjectWorkflowPreset {
    UMI_DEVELOPER_PROJECT_WORKFLOW_BUILD = 1,
    UMI_DEVELOPER_PROJECT_WORKFLOW_TEST = 2,
    UMI_DEVELOPER_PROJECT_WORKFLOW_RUN = 3,
    UMI_DEVELOPER_PROJECT_WORKFLOW_DEBUG = 4,
    UMI_DEVELOPER_PROJECT_WORKFLOW_FULL = 5
} UmiDeveloperProjectWorkflowPreset;

typedef struct UmiDeveloperProjectWorkflowRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDeveloperProjectWorkflowPreset preset;
    const char *workflow_id;
    const char *project_id;
    const char *configuration_id;
    const char *target_id;
    const char *task_id;
    const char *launch_profile_id;
    const char *environment_id;
    uint32_t timeout_ms;
    uint32_t max_attempts;
    int include_configure;
} UmiDeveloperProjectWorkflowRequest;

typedef struct UmiDeveloperProjectWorkflowSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDeveloperProjectWorkflowPreset preset;
    UmiProjectWorkspaceSelectionSnapshot selection;
    UmiProjectWorkspaceValidationReport validation;
    UmiDeveloperWorkflowSnapshot workflow;
    char task_ids[3][UMI_DEVELOPER_ID_CAPACITY];
    size_t task_count;
    int uses_launch_profile;
} UmiDeveloperProjectWorkflowSnapshot;

UmiStatus umi_developer_project_workflow_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperProjectWorkflowRequest *request,
    UmiDeveloperProjectWorkflowSnapshot *out_workflow);

#ifdef __cplusplus
}
#endif
#endif
