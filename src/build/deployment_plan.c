/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/deployment_plan.c
 *
 * PURPOSE:
 *   Implement immutable deployment planning, approval and default step
 *   composition from verified artifact and destination evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/build/deployment_plan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

static void write_message(char *destination, size_t capacity,
                          const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

void umi_build_deployment_step_init(
    UmiBuildDeploymentStepSnapshot *step,
    const char *step_id,
    const char *label,
    UmiBuildDeploymentStepKind kind)
{
    if (step == NULL) return;
    (void)memset(step, 0, sizeof(*step));
    step->structure_size = (uint32_t)sizeof(*step);
    step->api_version = UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION;
    if (step_id != NULL)
        (void)copy_text(step->step_id, sizeof(step->step_id), step_id);
    if (label != NULL)
        (void)copy_text(step->label, sizeof(step->label), label);
    step->kind = kind;
    step->state = UMI_BUILD_DEPLOYMENT_STEP_PENDING;
    step->timeout_ms = 300000U;
    step->required = 1;
    step->rollback_step = kind == UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK;
    step->revision = 1U;
}

void umi_build_deployment_plan_init(
    UmiBuildDeploymentPlan *plan,
    const char *deployment_id)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->structure_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION;
    if (deployment_id != NULL)
        (void)copy_text(plan->deployment_id,
                        sizeof(plan->deployment_id), deployment_id);
    plan->strategy = UMI_BUILD_DEPLOYMENT_COPY;
    plan->generation = 1U;
    plan->revision = 1U;
}


/* Public structs may come from adapters or saved state. Check every bounded
 * field before comparison, traversal or copying; partial plans remain editable. */
static int DeploymentText(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL;
}
static UmiStatus DeploymentStepShape(const UmiBuildDeploymentStepSnapshot *step)
{
    if (step == NULL || step->structure_size != sizeof(*step) ||
        step->api_version != UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION ||
        !DeploymentText(step->step_id, sizeof(step->step_id)) ||
        !DeploymentText(step->label, sizeof(step->label)) ||
        !DeploymentText(step->command, sizeof(step->command)) ||
        !DeploymentText(step->working_directory, sizeof(step->working_directory)) ||
        step->step_id[0] == '\0' || step->label[0] == '\0' ||
        step->kind < UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT ||
        step->kind > UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK ||
        step->state < UMI_BUILD_DEPLOYMENT_STEP_PENDING ||
        step->state > UMI_BUILD_DEPLOYMENT_STEP_SKIPPED ||
        (step->required != 0 && step->required != 1) ||
        (step->rollback_step != 0 && step->rollback_step != 1) || step->revision == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiStatus DeploymentPlanShape(const UmiBuildDeploymentPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->step_count > UMI_BUILD_DEPLOYMENT_STEP_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (plan->structure_size != sizeof(*plan) ||
        plan->api_version != UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION ||
        !DeploymentText(plan->deployment_id, sizeof(plan->deployment_id)) ||
        !DeploymentText(plan->artifact_id, sizeof(plan->artifact_id)) ||
        !DeploymentText(plan->target_id, sizeof(plan->target_id)) ||
        !DeploymentText(plan->environment, sizeof(plan->environment)) ||
        plan->strategy < UMI_BUILD_DEPLOYMENT_COPY || plan->strategy > UMI_BUILD_DEPLOYMENT_IN_PLACE ||
        (plan->dry_run != 0 && plan->dry_run != 1) ||
        (plan->requires_approval != 0 && plan->requires_approval != 1) ||
        (plan->approved != 0 && plan->approved != 1) ||
        (plan->rollback_enabled != 0 && plan->rollback_enabled != 1) ||
        plan->revision == 0U || plan->generation == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < plan->step_count; ++i) {
        UmiStatus status = DeploymentStepShape(&plan->steps[i]);
        if (status != UMI_STATUS_OK) return status;
        if (plan->steps[i].order != i) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Validate the complete owner shape and revision before rebinding its copied identities. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_bind(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target)
{
    char message[256];
    UmiStatus status;
    if (plan == NULL || artifact == NULL || target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_build_artifact_manifest_validate(
        artifact, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_target_validate(
        target, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    if ((artifact->flags & UMI_BUILD_ARTIFACT_DEPLOYABLE) == 0U ||
        (artifact->flags & UMI_BUILD_ARTIFACT_PRESENT) == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_ENABLED) == 0U) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = copy_text(plan->artifact_id, sizeof(plan->artifact_id),
                       artifact->artifact_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->target_id, sizeof(plan->target_id),
                           target->target_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->environment, sizeof(plan->environment),
                           target->environment);
    }
    if (status != UMI_STATUS_OK) return status;
    plan->strategy = target->strategy;
    plan->requires_approval =
        (target->flags & UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL) != 0U;
    plan->rollback_enabled =
        (target->flags & UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK) != 0U;
    plan->approved = !plan->requires_approval;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_bind(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target)
{
    char message[256];
    UmiStatus status;
    if (plan == NULL || artifact == NULL || target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = DeploymentPlanShape(plan);
    if (status != UMI_STATUS_OK) return status;
    if (plan->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_build_artifact_manifest_validate(
        artifact, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_target_validate(
        target, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    if ((artifact->flags & UMI_BUILD_ARTIFACT_DEPLOYABLE) == 0U ||
        (artifact->flags & UMI_BUILD_ARTIFACT_PRESENT) == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_ENABLED) == 0U) {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = copy_text(plan->artifact_id, sizeof(plan->artifact_id),
                       artifact->artifact_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->target_id, sizeof(plan->target_id),
                           target->target_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->environment, sizeof(plan->environment),
                           target->environment);
    }
    if (status != UMI_STATUS_OK) return status;
    plan->strategy = target->strategy;
    plan->requires_approval =
        (target->flags & UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL) != 0U;
    plan->rollback_enabled =
        (target->flags & UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK) != 0U;
    plan->approved = !plan->requires_approval;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}

/* Bound caller counts before traversal and invalidate content-specific approval when adding a step. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_add_step(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildDeploymentStepSnapshot *step)
{
    UmiBuildDeploymentStepSnapshot copy;
    size_t index;
    if (plan == NULL || step == NULL || step->step_id[0] == '\0' ||
        step->label[0] == '\0' ||
        step->kind < UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT ||
        step->kind > UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < plan->step_count; ++index) {
        if (strcmp(plan->steps[index].step_id, step->step_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (plan->step_count >= UMI_BUILD_DEPLOYMENT_STEP_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    copy = *step;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION;
    copy.order = plan->step_count;
    copy.state = UMI_BUILD_DEPLOYMENT_STEP_PENDING;
    copy.rollback_step =
        copy.rollback_step || copy.kind == UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK;
    if (copy.revision == 0U) copy.revision = 1U;
    plan->steps[plan->step_count++] = copy;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_add_step(
    UmiBuildDeploymentPlan *plan, const UmiBuildDeploymentStepSnapshot *step)
{
    UmiStatus status = DeploymentPlanShape(plan);
    if (status != UMI_STATUS_OK) return status;
    status = DeploymentStepShape(step);
    if (status != UMI_STATUS_OK) return status;
    if (plan->step_count == UMI_BUILD_DEPLOYMENT_STEP_CAPACITY || plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < plan->step_count; ++i)
        if (strcmp(plan->steps[i].step_id, step->step_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    UmiBuildDeploymentStepSnapshot copy = *step;
    copy.order = plan->step_count;
    copy.state = UMI_BUILD_DEPLOYMENT_STEP_PENDING;
    copy.rollback_step = copy.rollback_step || copy.kind == UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK;
    plan->steps[plan->step_count++] = copy;
    /* Approval describes the reviewed content; a new step needs a new review. */
    plan->approved = !plan->requires_approval;
    ++plan->revision;
    return UMI_STATUS_OK;
}

static UmiStatus add_default_step(UmiBuildDeploymentPlan *plan,
                                  const char *step_id,
                                  const char *label,
                                  UmiBuildDeploymentStepKind kind,
                                  const char *command,
                                  const char *working_directory,
                                  uint32_t timeout_ms,
                                  int required)
{
    UmiBuildDeploymentStepSnapshot step;
    UmiStatus status;
    umi_build_deployment_step_init(&step, step_id, label, kind);
    step.timeout_ms = timeout_ms;
    step.required = required != 0;
    if (command != NULL) {
        status = copy_text(step.command, sizeof(step.command), command);
        if (status != UMI_STATUS_OK) return status;
    }
    if (working_directory != NULL) {
        status = copy_text(step.working_directory,
                           sizeof(step.working_directory), working_directory);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_build_deployment_plan_add_step(plan, &step);
}

/* Stage the existing composition privately and publish once; refusal must preserve the previous plan. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_compose_default(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target,
    int dry_run)
{
    char deployment_id[UMI_BUILD_ID_CAPACITY];
    char command[UMI_BUILD_COMMAND_CAPACITY];
    UmiStatus status;
    if (plan == NULL || artifact == NULL || target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->deployment_id[0] == '\0') {
        (void)snprintf(deployment_id, sizeof(deployment_id),
                       "deploy.%.40s.%.40s", artifact->artifact_id, target->target_id);
    } else {
        (void)copy_text(deployment_id, sizeof(deployment_id),
                        plan->deployment_id);
    }
    umi_build_deployment_plan_init(plan, deployment_id);
    plan->dry_run = dry_run != 0;
    status = umi_build_deployment_plan_bind(plan, artifact, target);
    if (status != UMI_STATUS_OK) return status;

    (void)snprintf(command, sizeof(command),
                   "artifact.validate --id %s", artifact->artifact_id);
    status = add_default_step(plan, "validate-artifact", "Validate artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT,
                              command, NULL, target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "prepare-target", "Prepare target",
                              UMI_BUILD_DEPLOYMENT_STEP_PREPARE,
                              "deployment.prepare", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "stage-artifact", "Stage artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_STAGE,
                              "deployment.stage", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;

    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_REMOTE) != 0U) {
        status = add_default_step(plan, "transfer-artifact", "Transfer artifact",
                                  UMI_BUILD_DEPLOYMENT_STEP_TRANSFER,
                                  "deployment.transfer", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    if (target->kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
        target->kind == UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS ||
        target->kind == UMI_BUILD_DEPLOYMENT_CONTAINER ||
        target->strategy == UMI_BUILD_DEPLOYMENT_IN_PLACE ||
        target->strategy == UMI_BUILD_DEPLOYMENT_REPLACE) {
        status = add_default_step(plan, "stop-current", "Stop current generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_STOP,
                                  "deployment.stop", target->working_directory,
                                  target->timeout_ms, 0);
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_default_step(plan, "install-artifact", "Install artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_INSTALL,
                              "deployment.install", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "configure-generation",
                              "Configure generation",
                              UMI_BUILD_DEPLOYMENT_STEP_CONFIGURE,
                              "deployment.configure", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    if (target->kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
        target->kind == UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS ||
        target->kind == UMI_BUILD_DEPLOYMENT_CONTAINER) {
        status = add_default_step(plan, "start-generation", "Start generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_START,
                                  "deployment.start", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK) != 0U) {
        (void)snprintf(command, sizeof(command),
                       "deployment.health --uri %s", target->health_uri);
        status = add_default_step(plan, "verify-health", "Verify health",
                                  UMI_BUILD_DEPLOYMENT_STEP_HEALTH_CHECK,
                                  command, target->working_directory,
                                  target->health_timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_default_step(plan, "promote-generation", "Promote generation",
                              UMI_BUILD_DEPLOYMENT_STEP_PROMOTE,
                              "deployment.promote", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "cleanup-staging", "Clean staging",
                              UMI_BUILD_DEPLOYMENT_STEP_CLEANUP,
                              "deployment.cleanup", target->working_directory,
                              target->timeout_ms, 0);
    if (status != UMI_STATUS_OK) return status;
    if (plan->rollback_enabled) {
        status = add_default_step(plan, "rollback-generation",
                                  "Rollback generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK,
                                  "deployment.rollback", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
#endif

static UmiStatus ComposeDeploymentCandidate(
    UmiBuildDeploymentPlan *plan,
    const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target,
    int dry_run)
{
    char deployment_id[UMI_BUILD_ID_CAPACITY];
    char command[UMI_BUILD_COMMAND_CAPACITY];
    UmiStatus status;
    if (plan == NULL || artifact == NULL || target == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->deployment_id[0] == '\0') {
        (void)snprintf(deployment_id, sizeof(deployment_id),
                       "deploy.%.40s.%.40s", artifact->artifact_id, target->target_id);
    } else {
        (void)copy_text(deployment_id, sizeof(deployment_id),
                        plan->deployment_id);
    }
    umi_build_deployment_plan_init(plan, deployment_id);
    plan->dry_run = dry_run != 0;
    status = umi_build_deployment_plan_bind(plan, artifact, target);
    if (status != UMI_STATUS_OK) return status;

    (void)snprintf(command, sizeof(command),
                   "artifact.validate --id %s", artifact->artifact_id);
    status = add_default_step(plan, "validate-artifact", "Validate artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT,
                              command, NULL, target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "prepare-target", "Prepare target",
                              UMI_BUILD_DEPLOYMENT_STEP_PREPARE,
                              "deployment.prepare", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "stage-artifact", "Stage artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_STAGE,
                              "deployment.stage", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;

    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_REMOTE) != 0U) {
        status = add_default_step(plan, "transfer-artifact", "Transfer artifact",
                                  UMI_BUILD_DEPLOYMENT_STEP_TRANSFER,
                                  "deployment.transfer", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    if (target->kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
        target->kind == UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS ||
        target->kind == UMI_BUILD_DEPLOYMENT_CONTAINER ||
        target->strategy == UMI_BUILD_DEPLOYMENT_IN_PLACE ||
        target->strategy == UMI_BUILD_DEPLOYMENT_REPLACE) {
        status = add_default_step(plan, "stop-current", "Stop current generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_STOP,
                                  "deployment.stop", target->working_directory,
                                  target->timeout_ms, 0);
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_default_step(plan, "install-artifact", "Install artifact",
                              UMI_BUILD_DEPLOYMENT_STEP_INSTALL,
                              "deployment.install", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "configure-generation",
                              "Configure generation",
                              UMI_BUILD_DEPLOYMENT_STEP_CONFIGURE,
                              "deployment.configure", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    if (target->kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
        target->kind == UMI_BUILD_DEPLOYMENT_LOCAL_PROCESS ||
        target->kind == UMI_BUILD_DEPLOYMENT_CONTAINER) {
        status = add_default_step(plan, "start-generation", "Start generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_START,
                                  "deployment.start", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    if ((target->flags & UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK) != 0U) {
        (void)snprintf(command, sizeof(command),
                       "deployment.health --uri %s", target->health_uri);
        status = add_default_step(plan, "verify-health", "Verify health",
                                  UMI_BUILD_DEPLOYMENT_STEP_HEALTH_CHECK,
                                  command, target->working_directory,
                                  target->health_timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_default_step(plan, "promote-generation", "Promote generation",
                              UMI_BUILD_DEPLOYMENT_STEP_PROMOTE,
                              "deployment.promote", target->working_directory,
                              target->timeout_ms, 1);
    if (status != UMI_STATUS_OK) return status;
    status = add_default_step(plan, "cleanup-staging", "Clean staging",
                              UMI_BUILD_DEPLOYMENT_STEP_CLEANUP,
                              "deployment.cleanup", target->working_directory,
                              target->timeout_ms, 0);
    if (status != UMI_STATUS_OK) return status;
    if (plan->rollback_enabled) {
        status = add_default_step(plan, "rollback-generation",
                                  "Rollback generation",
                                  UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK,
                                  "deployment.rollback", target->working_directory,
                                  target->timeout_ms, 1);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_deployment_plan_compose_default(
    UmiBuildDeploymentPlan *plan, const UmiBuildArtifactManifest *artifact,
    const UmiBuildDeploymentTargetSnapshot *target, int dry_run)
{
    UmiStatus status = DeploymentPlanShape(plan);
    if (status != UMI_STATUS_OK) return status;
    if (artifact == NULL || target == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->revision == UINT64_MAX || plan->generation == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_build_artifact_manifest_validate(artifact, NULL, 0U);
    if (status == UMI_STATUS_OK) status = umi_build_deployment_target_validate(target, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    UmiBuildDeploymentPlan *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *plan;
    status = ComposeDeploymentCandidate(candidate, artifact, target, dry_run);
    if (status == UMI_STATUS_OK) status = umi_build_deployment_plan_validate(candidate, NULL, 0U);
    if (status == UMI_STATUS_OK) {
        candidate->revision = plan->revision + 1U;
        candidate->generation = plan->generation + 1U;
        *plan = *candidate;
    }
    free(candidate);
    return status;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_build_deployment_plan_approve(UmiBuildDeploymentPlan *plan)
{
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!plan->requires_approval) {
        plan->approved = 1;
        return UMI_STATUS_OK;
    }
    if (plan->deployment_id[0] == '\0' || plan->step_count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->approved = 1;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

/* Approval is only meaningful for a completely admitted plan. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_approve(UmiBuildDeploymentPlan *plan)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (plan != NULL && plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!plan->requires_approval) {
        plan->approved = 1;
        return UMI_STATUS_OK;
    }
    if (plan->deployment_id[0] == '\0' || plan->step_count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->approved = 1;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_approve(UmiBuildDeploymentPlan *plan)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (plan != NULL && plan->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    UmiStatus status = umi_build_deployment_plan_validate(plan, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    if (!plan->requires_approval) {
        plan->approved = 1;
        return UMI_STATUS_OK;
    }
    if (plan->deployment_id[0] == '\0' || plan->step_count == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    plan->approved = 1;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}

/* Validate bounded fields before comparison and require forward validation, installation and promotion in order. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_validate(
    const UmiBuildDeploymentPlan *plan,
    char *out_message,
    size_t message_capacity)
{
    size_t index;
    int has_validation = 0;
    int has_install = 0;
    int has_promotion = 0;
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->deployment_id[0] == '\0' || plan->artifact_id[0] == '\0' ||
        plan->target_id[0] == '\0' || plan->step_count == 0U) {
        write_message(out_message, message_capacity,
                      "Deployment identity, bindings and steps are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->step_count > UMI_BUILD_DEPLOYMENT_STEP_CAPACITY) {
        write_message(out_message, message_capacity,
                      "Deployment step capacity is exceeded.");
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < plan->step_count; ++index) {
        const UmiBuildDeploymentStepSnapshot *step = &plan->steps[index];
        size_t other;
        if (step->step_id[0] == '\0' || step->label[0] == '\0' ||
            step->order != index ||
            step->kind < UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT ||
            step->kind > UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK) {
            write_message(out_message, message_capacity,
                          "A deployment step is malformed or out of order.");
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (other = index + 1U; other < plan->step_count; ++other) {
            if (strcmp(step->step_id, plan->steps[other].step_id) == 0) {
                write_message(out_message, message_capacity,
                              "Deployment step ids must be unique.");
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT)
            has_validation = 1;
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_INSTALL)
            has_install = 1;
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_PROMOTE)
            has_promotion = 1;
    }
    if (!has_validation || !has_install || !has_promotion) {
        write_message(out_message, message_capacity,
                      "Default safety gates are missing.");
        return UMI_STATUS_INVALID_STATE;
    }
    write_message(out_message, message_capacity, "Deployment plan is valid.");
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_validate(
    const UmiBuildDeploymentPlan *plan, char *out_message, size_t message_capacity)
{
    UmiStatus status = DeploymentPlanShape(plan);
    if (status != UMI_STATUS_OK) {
        write_message(out_message, message_capacity, "Deployment storage or metadata is malformed.");
        return status;
    }
    if (plan->deployment_id[0] == '\0' || plan->artifact_id[0] == '\0' ||
        plan->target_id[0] == '\0' || plan->step_count == 0U) {
        write_message(out_message, message_capacity, "Deployment identity, bindings and steps are required.");
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    int validated = 0, installed = 0, promoted = 0;
    for (size_t i = 0U; i < plan->step_count; ++i) {
        const UmiBuildDeploymentStepSnapshot *step = &plan->steps[i];
        for (size_t j = i + 1U; j < plan->step_count; ++j)
            if (strcmp(step->step_id, plan->steps[j].step_id) == 0) {
                write_message(out_message, message_capacity, "Deployment step ids must be unique.");
                return UMI_STATUS_ALREADY_EXISTS;
            }
        int gate = step->kind == UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT ||
                   step->kind == UMI_BUILD_DEPLOYMENT_STEP_INSTALL ||
                   step->kind == UMI_BUILD_DEPLOYMENT_STEP_PROMOTE;
        if (step->rollback_step) continue;
        if (gate && !step->required) {
            write_message(out_message, message_capacity, "Safety gates must be required forward steps.");
            return UMI_STATUS_INVALID_STATE;
        }
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT) validated = 1;
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_INSTALL) {
            if (!validated) return UMI_STATUS_INVALID_STATE;
            installed = 1;
        }
        if (step->kind == UMI_BUILD_DEPLOYMENT_STEP_PROMOTE) {
            if (!installed) return UMI_STATUS_INVALID_STATE;
            promoted = 1;
        }
    }
    if (!validated || !installed || !promoted) {
        write_message(out_message, message_capacity, "Ordered forward safety gates are missing.");
        return UMI_STATUS_INVALID_STATE;
    }
    write_message(out_message, message_capacity, "Deployment plan is valid.");
    return UMI_STATUS_OK;
}

/* Inspect bounds before projecting caller-owned plan data; rejected reads leave outputs unchanged. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_step_at(
    const UmiBuildDeploymentPlan *plan,
    size_t position,
    UmiBuildDeploymentStepSnapshot *out_step)
{
    if (plan == NULL || out_step == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= plan->step_count) return UMI_STATUS_NOT_FOUND;
    *out_step = plan->steps[position];
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_step_at(
    const UmiBuildDeploymentPlan *plan,
    size_t position,
    UmiBuildDeploymentStepSnapshot *out_step)
{
    UmiStatus shape_status = DeploymentPlanShape(plan);
    if (shape_status != UMI_STATUS_OK) return shape_status;
    if (plan == NULL || out_step == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (position >= plan->step_count) return UMI_STATUS_NOT_FOUND;
    *out_step = plan->steps[position];
    return UMI_STATUS_OK;
}

/* Inspect bounds before projecting caller-owned plan data; rejected reads leave outputs unchanged. The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_deployment_plan_snapshot(
    const UmiBuildDeploymentPlan *plan,
    UmiBuildDeploymentPlanSnapshot *out_snapshot)
{
    char message[256];
    size_t index;
    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION;
    (void)copy_text(out_snapshot->deployment_id,
                    sizeof(out_snapshot->deployment_id), plan->deployment_id);
    (void)copy_text(out_snapshot->artifact_id,
                    sizeof(out_snapshot->artifact_id), plan->artifact_id);
    (void)copy_text(out_snapshot->target_id,
                    sizeof(out_snapshot->target_id), plan->target_id);
    (void)copy_text(out_snapshot->environment,
                    sizeof(out_snapshot->environment), plan->environment);
    out_snapshot->strategy = plan->strategy;
    out_snapshot->step_count = plan->step_count;
    out_snapshot->generation = plan->generation;
    out_snapshot->revision = plan->revision;
    out_snapshot->dry_run = plan->dry_run;
    out_snapshot->requires_approval = plan->requires_approval;
    out_snapshot->approved = plan->approved;
    out_snapshot->rollback_enabled = plan->rollback_enabled;
    for (index = 0U; index < plan->step_count; ++index) {
        if (plan->steps[index].required) out_snapshot->required_count += 1U;
        if (plan->steps[index].rollback_step)
            out_snapshot->rollback_step_count += 1U;
    }
    out_snapshot->valid =
        umi_build_deployment_plan_validate(plan, message, sizeof(message)) ==
        UMI_STATUS_OK;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_build_deployment_plan_snapshot(
    const UmiBuildDeploymentPlan *plan,
    UmiBuildDeploymentPlanSnapshot *out_snapshot)
{
    UmiStatus shape_status = DeploymentPlanShape(plan);
    if (shape_status != UMI_STATUS_OK) return shape_status;
    char message[256];
    size_t index;
    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_DEPLOYMENT_PLAN_API_VERSION;
    (void)copy_text(out_snapshot->deployment_id,
                    sizeof(out_snapshot->deployment_id), plan->deployment_id);
    (void)copy_text(out_snapshot->artifact_id,
                    sizeof(out_snapshot->artifact_id), plan->artifact_id);
    (void)copy_text(out_snapshot->target_id,
                    sizeof(out_snapshot->target_id), plan->target_id);
    (void)copy_text(out_snapshot->environment,
                    sizeof(out_snapshot->environment), plan->environment);
    out_snapshot->strategy = plan->strategy;
    out_snapshot->step_count = plan->step_count;
    out_snapshot->generation = plan->generation;
    out_snapshot->revision = plan->revision;
    out_snapshot->dry_run = plan->dry_run;
    out_snapshot->requires_approval = plan->requires_approval;
    out_snapshot->approved = plan->approved;
    out_snapshot->rollback_enabled = plan->rollback_enabled;
    for (index = 0U; index < plan->step_count; ++index) {
        if (plan->steps[index].required) out_snapshot->required_count += 1U;
        if (plan->steps[index].rollback_step)
            out_snapshot->rollback_step_count += 1U;
    }
    out_snapshot->valid =
        umi_build_deployment_plan_validate(plan, message, sizeof(message)) ==
        UMI_STATUS_OK;
    return UMI_STATUS_OK;
}

const char *umi_build_deployment_step_kind_text(
    UmiBuildDeploymentStepKind kind)
{
    switch (kind) {
    case UMI_BUILD_DEPLOYMENT_STEP_VALIDATE_ARTIFACT: return "Validate artifact";
    case UMI_BUILD_DEPLOYMENT_STEP_PREPARE: return "Prepare";
    case UMI_BUILD_DEPLOYMENT_STEP_STAGE: return "Stage";
    case UMI_BUILD_DEPLOYMENT_STEP_TRANSFER: return "Transfer";
    case UMI_BUILD_DEPLOYMENT_STEP_STOP: return "Stop";
    case UMI_BUILD_DEPLOYMENT_STEP_INSTALL: return "Install";
    case UMI_BUILD_DEPLOYMENT_STEP_CONFIGURE: return "Configure";
    case UMI_BUILD_DEPLOYMENT_STEP_START: return "Start";
    case UMI_BUILD_DEPLOYMENT_STEP_HEALTH_CHECK: return "Health check";
    case UMI_BUILD_DEPLOYMENT_STEP_PROMOTE: return "Promote";
    case UMI_BUILD_DEPLOYMENT_STEP_CLEANUP: return "Cleanup";
    case UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK: return "Rollback";
    default: return "Unknown";
    }
}

const char *umi_build_deployment_step_state_text(
    UmiBuildDeploymentStepState state)
{
    switch (state) {
    case UMI_BUILD_DEPLOYMENT_STEP_PENDING: return "Pending";
    case UMI_BUILD_DEPLOYMENT_STEP_RUNNING: return "Running";
    case UMI_BUILD_DEPLOYMENT_STEP_SUCCEEDED: return "Succeeded";
    case UMI_BUILD_DEPLOYMENT_STEP_FAILED: return "Failed";
    case UMI_BUILD_DEPLOYMENT_STEP_CANCELLED: return "Cancelled";
    case UMI_BUILD_DEPLOYMENT_STEP_SKIPPED: return "Skipped";
    default: return "Unknown";
    }
}
