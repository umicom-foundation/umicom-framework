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

typedef enum UmiDeveloperLifecycleGoal {
    UMI_DEVELOPER_LIFECYCLE_VERIFY = 1,
    UMI_DEVELOPER_LIFECYCLE_BUILD = 2,
    UMI_DEVELOPER_LIFECYCLE_TEST = 3,
    UMI_DEVELOPER_LIFECYCLE_PUBLISH = 4,
    UMI_DEVELOPER_LIFECYCLE_UPDATE = 5,
    UMI_DEVELOPER_LIFECYCLE_COMPLETE = 6
} UmiDeveloperLifecycleGoal;

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

typedef struct UmiDeveloperLifecyclePlanRequest {
    UmiDeveloperLifecycleGoal goal;
    int require_external_vulnerability_scanner;
} UmiDeveloperLifecyclePlanRequest;

typedef struct UmiDeveloperLifecyclePlanStage {
    UmiDeveloperLifecycleStage stage;
    int required;
    int changes_local_state;
    int uses_network;
    const char *title;
    const char *explanation;
} UmiDeveloperLifecyclePlanStage;

typedef struct UmiDeveloperLifecyclePlan {
    UmiDeveloperLifecycleGoal goal;
    UmiDeveloperLifecyclePlanStage stages[UMI_DEVELOPER_LIFECYCLE_STAGE_CAPACITY];
    size_t stage_count;
} UmiDeveloperLifecyclePlan;

void umi_developer_lifecycle_plan_request_init(
    UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecycleGoal goal);
UmiStatus umi_developer_lifecycle_plan_build(
    const UmiDeveloperLifecyclePlanRequest *request,
    UmiDeveloperLifecyclePlan *out_plan);
const char *umi_developer_lifecycle_goal_text(UmiDeveloperLifecycleGoal goal);
const char *umi_developer_lifecycle_stage_text(UmiDeveloperLifecycleStage stage);

#ifdef __cplusplus
}
#endif

#endif
