/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/workflow.h
 *
 * PURPOSE:
 *   Compose a named, dependency-ordered workflow from existing project tasks
 *   and an optional run/debug launch profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKFLOW_H
#define UMICOM_DEVELOPER_WORKFLOW_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/developer/launch_plan.h"
#include "umicom/developer/task_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_WORKFLOW_API_VERSION 1U
#define UMI_DEVELOPER_WORKFLOW_MAX_TASKS 16U

/**
 * Represent the developer workflow request data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkflowRequest {
    uint32_t struct_size;
    uint32_t api_version;
    const char *workflow_id;
    const char *const *task_ids;
    size_t task_count;
    const char *launch_profile_id;
    const char *configuration_id;
    const char *target_id;
    uint32_t timeout_ms;
    uint32_t max_attempts;
    int debug_mode;
} UmiDeveloperWorkflowRequest;

/**
 * Represent the developer workflow snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkflowSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char workflow_id[UMI_DEVELOPER_ID_CAPACITY];
    char operation_ids[UMI_DEVELOPER_WORKFLOW_MAX_TASKS + 1U][UMI_DEVELOPER_ID_CAPACITY];
    size_t operation_count;
    size_t dependency_count;
} UmiDeveloperWorkflowSnapshot;

/**
 * Provide the developer workflow submit operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_workflow_submit(
    UmiDeveloperRuntime *runtime,
    const UmiDeveloperWorkflowRequest *request,
    UmiDeveloperWorkflowSnapshot *out_workflow);

#ifdef __cplusplus
}
#endif

#endif
