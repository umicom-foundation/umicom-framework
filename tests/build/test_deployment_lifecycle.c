/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_deployment_lifecycle.c
 *
 * PURPOSE:
 *   Exercise deployment completion, cancellation, rollback and atomic revision refusal through the
 * actual controller implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/deployment_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Include the production owner to place its private counters at exhaustion.
 * Normal lifecycle assertions still use the public API. No provider runs. */
#include "../../src/build/deployment_controller.c"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                              \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static UmiStatus Make(UmiBuildDeploymentPlan *plan) {
  UmiBuildArtifactManifest artifact;
  UmiBuildDeploymentTargetSnapshot target;
  umi_build_artifact_manifest_init(&artifact, "lesson", "dist/lesson.zip", "archive");
  artifact.flags |= UMI_BUILD_ARTIFACT_DEPLOYABLE;
  umi_build_deployment_target_init(&target, "local", "Local", UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY);
  strcpy(target.destination, "install/lesson");
  target.flags |=
      UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL | UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK;
  umi_build_deployment_plan_init(plan, "lesson.deploy");
  return umi_build_deployment_plan_compose_default(plan, &artifact, &target, 1);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  const char *cases[] = {"normal-completion",
                         "rollback-completion",
                         "rollback-failure",
                         "next-busy",
                         "cancel-idle",
                         "cancel-running",
                         "failed-step",
                         "approval-required",
                         "load-owned",
                         "malformed-load",
                         "load-revision",
                         "load-step-revision",
                         "begin-revision",
                         "start-revision",
                         "start-step-revision",
                         "complete-revision",
                         "complete-step-revision",
                         "fail-revision",
                         "fail-step-revision",
                         "stop-revision",
                         "stop-step-revision",
                         "rollback-revision",
                         "rollback-step-revision",
                         "finish-rollback-revision"};
  size_t known = 0U;
  for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
    known += (size_t)Is(mode, cases[i]);
  if (known != 1U)
    return 2;
  UmiBuildDeploymentPlan *plan = calloc(1U, sizeof(*plan));
  CHECK(plan != NULL);
  CHECK(Make(plan) == UMI_STATUS_OK);
  if (!Is(mode, "approval-required"))
    CHECK(umi_build_deployment_plan_approve(plan) == UMI_STATUS_OK);
  UmiBuildDeploymentController *owner = NULL;
  CHECK(umi_build_deployment_controller_create(&owner) == UMI_STATUS_OK);
  UmiBuildDeploymentController *before = malloc(sizeof(*before));
  CHECK(before != NULL);
  UmiBuildDeploymentControllerSnapshot snapshot;
  UmiBuildDeploymentStepSnapshot step;
  if (Is(mode, "load-revision") || Is(mode, "load-step-revision") || Is(mode, "malformed-load")) {
    if (Is(mode, "load-revision"))
      owner->revision = UINT64_MAX;
    if (Is(mode, "load-step-revision"))
      plan->steps[plan->step_count - 1U].revision = UINT64_MAX;
    if (Is(mode, "malformed-load"))
      plan->step_count = SIZE_MAX;
    *before = *owner;
    CHECK(umi_build_deployment_controller_load(owner, plan) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_load(owner, plan) == UMI_STATUS_OK);
  if (Is(mode, "load-owned")) {
    plan->steps[0].label[0] = 'X';
    CHECK(owner->plan.steps[0].label[0] != 'X');
    goto done;
  }
  if (Is(mode, "approval-required")) {
    CHECK(umi_build_deployment_controller_begin(owner) == UMI_STATUS_PERMISSION_DENIED);
    goto done;
  }
  if (Is(mode, "begin-revision")) {
    owner->revision = UINT64_MAX;
    *before = *owner;
    CHECK(umi_build_deployment_controller_begin(owner) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_begin(owner) == UMI_STATUS_OK);
  CHECK(umi_build_deployment_controller_next_step(owner, &step) == UMI_STATUS_OK);
  if (Is(mode, "normal-completion")) {
    size_t completed = 0U;
    do {
      CHECK(umi_build_deployment_controller_start_step(owner, step.step_id) == UMI_STATUS_OK);
      CHECK(umi_build_deployment_controller_complete_step(owner, step.step_id, 0) == UMI_STATUS_OK);
      ++completed;
    } while (umi_build_deployment_controller_next_step(owner, &step) == UMI_STATUS_OK);
    CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_SUCCEEDED &&
          snapshot.progress_basis_points == 10000U);
    CHECK(snapshot.completed_step_count == completed && completed + 1U == plan->step_count);
    goto done;
  }
  if (Is(mode, "cancel-idle")) {
    CHECK(umi_build_deployment_controller_request_stop(owner) == UMI_STATUS_OK);
    CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_CANCELLED &&
          snapshot.last_status == UMI_STATUS_CANCELLED && snapshot.progress_basis_points == 10000U);
    goto done;
  }
  if (Is(mode, "start-revision") || Is(mode, "start-step-revision") || Is(mode, "stop-revision") ||
      Is(mode, "stop-step-revision")) {
    int stopping = strncmp(mode, "stop-", 5U) == 0;
    if (strstr(mode, "step-revision") != NULL)
      owner->plan.steps[stopping ? 2U : 0U].revision = UINT64_MAX;
    else
      owner->revision = UINT64_MAX;
    *before = *owner;
    CHECK((stopping ? umi_build_deployment_controller_request_stop(owner)
                    : umi_build_deployment_controller_start_step(owner, step.step_id)) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_start_step(owner, step.step_id) == UMI_STATUS_OK);
  if (Is(mode, "next-busy")) {
    UmiBuildDeploymentStepSnapshot out;
    memset(&out, 0xa5, sizeof(out));
    UmiBuildDeploymentStepSnapshot saved = out;
    CHECK(umi_build_deployment_controller_next_step(owner, &out) == UMI_STATUS_BUSY);
    CHECK(memcmp(&out, &saved, sizeof(out)) == 0);
    goto done;
  }
  if (Is(mode, "cancel-running")) {
    CHECK(umi_build_deployment_controller_request_stop(owner) == UMI_STATUS_OK);
    CHECK(umi_build_deployment_controller_complete_step(owner, step.step_id, 0) == UMI_STATUS_OK);
    CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_CANCELLED &&
          snapshot.last_status == UMI_STATUS_CANCELLED && snapshot.completed_step_count == 1U);
    goto done;
  }
  if (strncmp(mode, "complete-", 9U) == 0 || strncmp(mode, "fail-", 5U) == 0) {
    if (strstr(mode, "step-revision") != NULL)
      owner->plan.steps[0].revision = UINT64_MAX;
    else
      owner->revision = UINT64_MAX;
    *before = *owner;
    CHECK((strncmp(mode, "fail-", 5U) == 0
               ? umi_build_deployment_controller_fail_step(owner, step.step_id, UMI_STATUS_IO_ERROR,
                                                           7, "controlled failure")
               : umi_build_deployment_controller_complete_step(owner, step.step_id, 0)) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_complete_step(owner, step.step_id, 7) == UMI_STATUS_OK);
  if (Is(mode, "failed-step")) {
    CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_FAILED && snapshot.last_exit_code == 7 &&
          snapshot.failed_step_count == 1U);
    goto done;
  }
  if (Is(mode, "rollback-revision") || Is(mode, "rollback-step-revision")) {
    if (Is(mode, "rollback-revision"))
      owner->revision = UINT64_MAX;
    else
      owner->plan.steps[plan->step_count - 1U].revision = UINT64_MAX;
    *before = *owner;
    CHECK(umi_build_deployment_controller_begin_rollback(owner) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_begin_rollback(owner) == UMI_STATUS_OK);
  CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
  CHECK(snapshot.progress_basis_points == 0U);
  CHECK(umi_build_deployment_controller_next_step(owner, &step) == UMI_STATUS_OK &&
        step.rollback_step);
  CHECK(umi_build_deployment_controller_start_step(owner, step.step_id) == UMI_STATUS_OK);
  CHECK(umi_build_deployment_controller_complete_step(
            owner, step.step_id, Is(mode, "rollback-failure") ? 9 : 0) == UMI_STATUS_OK);
  if (Is(mode, "rollback-failure")) {
    CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_FAILED && snapshot.last_exit_code == 9 &&
          snapshot.progress_basis_points == 10000U);
    goto done;
  }
  if (Is(mode, "finish-rollback-revision")) {
    owner->revision = UINT64_MAX;
    *before = *owner;
    CHECK(umi_build_deployment_controller_complete_rollback(owner, 0) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(owner, before, sizeof(*owner)) == 0);
    goto done;
  }
  CHECK(umi_build_deployment_controller_complete_rollback(owner, 0) == UMI_STATUS_OK);
  CHECK(umi_build_deployment_controller_snapshot(owner, &snapshot) == UMI_STATUS_OK);
  CHECK(snapshot.state == UMI_BUILD_DEPLOYMENT_ROLLED_BACK &&
        snapshot.progress_basis_points == 10000U && snapshot.failed_step_count == 1U &&
        snapshot.completed_step_count == 1U);
done:
  free(before);
  umi_build_deployment_controller_destroy(owner);
  free(plan);
  return 0;
}
