/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/lifecycle_plan.h
 *
 * PURPOSE:
 *   Define an inspectable development lifecycle shared by the Umicom command,
 *   Studio task views and future automation without coupling the plan to a
 *   command shell or one operating system.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_LIFECYCLE_PLAN_H
#define UMICOM_DEVELOPER_LIFECYCLE_PLAN_H

#include <stddef.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_LIFECYCLE_STAGE_CAPACITY 20U

/**
 * List the named developer lifecycle goal values accepted by this public contract.
 */
typedef enum UmiDeveloperLifecycleGoal {
    UMI_DEVELOPER_LIFECYCLE_VERIFY = 1,
    UMI_DEVELOPER_LIFECYCLE_BUILD = 2,
    UMI_DEVELOPER_LIFECYCLE_TEST = 3,
    UMI_DEVELOPER_LIFECYCLE_PUBLISH = 4,
    UMI_DEVELOPER_LIFECYCLE_UPDATE = 5,
    UMI_DEVELOPER_LIFECYCLE_COMPLETE = 6
} UmiDeveloperLifecycleGoal;

/**
 * List the named developer lifecycle stage values accepted by this public contract.
 */
typedef enum UmiDeveloperLifecycleStage {
    UMI_DEVELOPER_STAGE_ENVIRONMENT = 1,
    UMI_DEVELOPER_STAGE_DEPENDENCIES = 2,
    UMI_DEVELOPER_STAGE_SOURCE_QUALITY = 3,
    UMI_DEVELOPER_STAGE_SOURCE_SECURITY = 4,
    UMI_DEVELOPER_STAGE_ARCHITECTURE = 5,
    UMI_DEVELOPER_STAGE_MEMORY_STATIC = 6,
    UMI_DEVELOPER_STAGE_DEPENDENCY_VULNERABILITIES = 7,
    UMI_DEVELOPER_STAGE_CONFIGURE = 8,
    UMI_DEVELOPER_STAGE_COMPILE_LINK = 9,
    UMI_DEVELOPER_STAGE_TEST = 10,
    UMI_DEVELOPER_STAGE_REPOSITORY_VERIFY = 11,
    UMI_DEVELOPER_STAGE_REPOSITORY_STAGE = 12,
    UMI_DEVELOPER_STAGE_REPOSITORY_COMMIT = 13,
    UMI_DEVELOPER_STAGE_REPOSITORY_PUSH = 14,
    UMI_DEVELOPER_STAGE_REPOSITORY_UPDATE = 15
} UmiDeveloperLifecycleStage;

/**
 * Represent the developer lifecycle plan request data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperLifecyclePlanRequest {
    UmiDeveloperLifecycleGoal goal;
    int require_external_vulnerability_scanner;
} UmiDeveloperLifecyclePlanRequest;

/**
 * Represent the developer lifecycle plan stage data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperLifecyclePlanStage {
    UmiDeveloperLifecycleStage stage;
    int required;
    int changes_local_state;
    int uses_network;
    const char *title;
    const char *explanation;
} UmiDeveloperLifecyclePlanStage;

/**
 * Represent the developer lifecycle plan data shared with callers of this public contract.
 */
typedef struct UmiDeveloperLifecyclePlan {
    UmiDeveloperLifecycleGoal goal;
    UmiDeveloperLifecyclePlanStage stages[UMI_DEVELOPER_LIFECYCLE_STAGE_CAPACITY];
    size_t stage_count;
} UmiDeveloperLifecyclePlan;

/**
 * Initialise developer lifecycle plan request from caller-provided values so later
 * operations receive a known state.
 */
void umi_developer_lifecycle_plan_request_init(
    UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecycleGoal goal);
/**
 * Provide the developer lifecycle plan build operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_lifecycle_plan_build(
    const UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecyclePlan *out_plan);
/**
 * Provide the developer lifecycle goal text operation used by this module and its client
 * applications.
 */
const char *umi_developer_lifecycle_goal_text(UmiDeveloperLifecycleGoal goal);
/**
 * Provide the developer lifecycle stage text operation used by this module and its client
 * applications.
 */
const char *umi_developer_lifecycle_stage_text(UmiDeveloperLifecycleStage stage);

#ifdef __cplusplus
}
#endif

#endif
