/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/deployment_plan.h
 *
 * PURPOSE:
 *   Compose an immutable, reviewable deployment plan from one verified build
 *   artifact and one deployment target before any external action is allowed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_DEPLOYMENT_PLAN_H
#define UMICOM_BUILD_DEPLOYMENT_PLAN_H

#include "umicom/build/artifact_manifest.h"
#include "umicom/build/deployment_target.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION 1U
#define UMI_BUILD_DEPLOYMENT_STEP_CAPACITY 32U

typedef enum UmiBuildDeploymentStepKind {
    UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT = 0,
    UMI_BUILD_DEPLOYMENT_STEP_PREPARE = 1,
    UMI_BUILD_DEPLOYMENT_STEP_STAGE = 2,
    UMI_BUILD_DEPLOYMENT_STEP_TRANSFER = 3,
    UMI_BUILD_DEPLOYMENT_STEP_STOP = 4,
    UMI_BUILD_DEPLOYMENT_STEP_INSTALL = 5,
    UMI_BUILD_DEPLOYMENT_STEP_CONFIGURE = 6,
    UMI_BUILD_DEPLOYMENT_STEP_START = 7,
    UMI_BUILD_DEPLOYMENT_STEP_HEALTH_CHECK = 8,
    UMI_BUILD_DEPLOYMENT_STEP_PROMOTE = 9,
    UMI_BUILD_DEPLOYMENT_STEP_CLEANUP = 10,
    UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK = 11
} UmiBuildDeploymentStepKind;

typedef enum UmiBuildDeploymentStepState {
    UMI_BUILD_DEPLOYMENT_STEP_PENDING = 0,
    UMI_BUILD_DEPLOYMENT_STEP_RUNNING = 1,
    UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED = 2,
    UMI_BUILD_DEPLOYMENT_STEP_FAILED = 3,
    UMI_BUILD_DEPLOYMENT_STEP_CANCELLED = 4,
    UMI_BUILD_DEPLOYMENT_STEP_SKIPPED = 5
} UmiBuildDeploymentStepState;

typedef struct UmiBuildDeploymentStepSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char step_id[UMI_BUILD_ID_CAPACITY];
    char label[UMI_BUILD_NAME_CAPACITY];
    char command[UMI_BUILD_COMMAND_CAPACITY];
    char working_directory[UMI_BUILD_PATH_CAPACITY];
    UmiBuildDeploymentStepKind kind;
    UmiBuildDeploymentStepState state;
    size_t order;
    uint32_t timeout_ms;
    int required;
    int rollback_step;
    uint64_t revision;
} UmiBuildDeploymentStepSnapshot;

typedef struct UmiBuildDeploymentPlan {
    uint32_t structure_size;
    uint32_t api_version;
    char deployment_id[UMI_BUILD_ID_CAPACITY];
    char artifact_id[UMI_BUILD_ID_CAPACITY];
    char target_id[UMI_BUILD_ID_CAPACITY];
    char environment[UMI_BUILD_NAME_CAPACITY];
    UmiBuildDeploymentStrategy strategy;
    UmiBuildDeploymentStepSnapshot steps[UMI_BUILD_DEPLOYMENT_STEP_CAPACITY];
    size_t step_count;
    uint64_t generation;
    uint64_t revision;
    int dry_run;
    int requires_approval;
    int approved;
    int rollback_enabled;
} UmiBuildDeploymentPlan;

typedef struct UmiBuildDeploymentPlanSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char deployment_id[UMI_BUILD_ID_CAPACITY];
    char artifact_id[UMI_BUILD_ID_CAPACITY];
    char target_id[UMI_BUILD_ID_CAPACITY];
    char environment[UMI_BUILD_NAME_CAPACITY];
    UmiBuildDeploymentStrategy strategy;
    size_t step_count;
    size_t required_count;
    size_t rollback_step_count;
    uint64_t generation;
    uint64_t revision;
    int dry_run;
    int requires_approval;
    int approved;
    int rollback_enabled;
    int valid;
} UmiBuildDeploymentPlanSnapshot;

void umi_build_deployment_plan_init(
    UmiBuildDeploymentPlan *plan,
    const char *deployment_id);
UmiStatus umi_build_deployment_plan_bind(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target);
UmiStatus umi_build_deployment_plan_add_step(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildDeploymentStepSnapshot *step);
UmiStatus umi_build_deployment_plan_compose_default(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target,
    int dry_run);
UmiStatus umi_build_deployment_plan_approve(UmiBuildDeploymentPlan *plan);
UmiStatus umi_build_deployment_plan_validate(
    const UmiBuildDeploymentPlan *plan,
    char *out_message,
    size_t message_capacity);
UmiStatus umi_build_deployment_plan_step_at(
    const UmiBuildDeploymentPlan *plan,
    size_t position,
    UmiBuildDeploymentStepSnapshot *out_step);
UmiStatus umi_build_deployment_plan_snapshot(
    const UmiBuildDeploymentPlan *plan,
    UmiBuildDeploymentPlanSnapshot *out_snapshot);
void umi_build_deployment_step_init(
    UmiBuildDeploymentStepSnapshot *step,
    const char *step_id,
    const char *label,
    UmiBuildDeploymentStepKind kind);
const char *umi_build_deployment_step_kind_text(
    UmiBuildDeploymentStepKind kind);
const char *umi_build_deployment_step_state_text(
    UmiBuildDeploymentStepState state);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_DEPLOYMENT_PLAN_H */
