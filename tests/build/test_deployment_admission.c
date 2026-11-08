/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_deployment_admission.c
 *
 * PURPOSE:
 *   Verify bounded public deployment values, transactional composition and content-specific
 * approval without executing providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/deployment_plan.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                              \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#define FIELD(type, field) {offsetof(type, field), sizeof(((type *)0)->field)}
typedef struct Field {
  size_t offset, capacity;
} Field;
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static UmiStatus Make(UmiBuildDeploymentPlan *plan, UmiBuildArtifactManifest *artifact,
                      UmiBuildDeploymentTargetSnapshot *target) {
  umi_build_artifact_manifest_init(artifact, "lesson", "dist/lesson.zip", "archive");
  artifact->flags |= UMI_BUILD_ARTIFACT_DEPLOYABLE;
  umi_build_deployment_target_init(target, "local", "Local", UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY);
  strcpy(target->destination, "install/lesson");
  target->flags |=
      UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL | UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK;
  umi_build_deployment_plan_init(plan, "lesson.deploy");
  return umi_build_deployment_plan_compose_default(plan, artifact, target, 1);
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  const char *cases[] = {"valid", "rollback-install-allowed",
                         "count-validate",
                         "count-snapshot",
                         "count-step",
                         "count-add",
                         "plan-text",
                         "step-text",
                         "artifact-text",
                         "target-text",
                         "plan-metadata",
                         "step-metadata",
                         "artifact-metadata",
                         "target-metadata",
                         "plan-bool",
                         "step-bool",
                         "plan-strategy",
                         "step-kind",
                         "step-state",
                         "plan-revision-zero",
                         "step-revision-zero",
                         "order",
                         "duplicate",
                         "rollback-gate",
                         "optional-gate",
                         "install-before-validation",
                         "promote-before-install",
                         "missing-gate",
                         "invalid-compose-preserves",
                         "disabled-target-preserves",
                         "invalid-approve-preserves",
                         "add-revokes-approval",
                         "bind-revokes-approval",
                         "compose-revision",
                         "bind-revision",
                         "add-revision",
                         "approve-revision",
                         "compose-generation",
                         "step-out-of-range",
                         "partial-snapshot",
                         "source-copy"};
  size_t known = 0U;
  for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
    known += (size_t)Is(mode, cases[i]);
  if (known != 1U)
    return 2;
  UmiBuildDeploymentPlan *plan = calloc(1U, sizeof(*plan)), *before = calloc(1U, sizeof(*before));
  CHECK(plan != NULL && before != NULL);
  UmiBuildArtifactManifest artifact;
  UmiBuildDeploymentTargetSnapshot target;
  CHECK(Make(plan, &artifact, &target) == UMI_STATUS_OK);
  UmiBuildDeploymentStepSnapshot step;
  umi_build_deployment_step_init(&step, "extra", "Extra", UMI_BUILD_DEPLOYMENT_STEP_CLEANUP);
  UmiBuildDeploymentPlanSnapshot snapshot;
  memset(&snapshot, 0xa5, sizeof(snapshot));
  UmiBuildDeploymentPlanSnapshot saved_snapshot = snapshot;
  UmiBuildDeploymentStepSnapshot saved_step = step;
  if (strncmp(mode, "count-", 6U) == 0) {
    plan->step_count = SIZE_MAX;
    *before = *plan;
    UmiStatus status;
    if (Is(mode, "count-snapshot"))
      status = umi_build_deployment_plan_snapshot(plan, &snapshot);
    else if (Is(mode, "count-step"))
      status = umi_build_deployment_plan_step_at(plan, 0U, &step);
    else if (Is(mode, "count-add"))
      status = umi_build_deployment_plan_add_step(plan, &step);
    else
      status = umi_build_deployment_plan_validate(plan, NULL, 0U);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
    CHECK(memcmp(&snapshot, &saved_snapshot, sizeof(snapshot)) == 0);
    CHECK(memcmp(&step, &saved_step, sizeof(step)) == 0);
  } else if (Is(mode, "plan-text") || Is(mode, "step-text") || Is(mode, "artifact-text") ||
             Is(mode, "target-text")) {
    static const Field pf[] = {
        FIELD(UmiBuildDeploymentPlan, deployment_id), FIELD(UmiBuildDeploymentPlan, artifact_id),
        FIELD(UmiBuildDeploymentPlan, target_id), FIELD(UmiBuildDeploymentPlan, environment)};
    static const Field sf[] = {FIELD(UmiBuildDeploymentStepSnapshot, step_id),
                               FIELD(UmiBuildDeploymentStepSnapshot, label),
                               FIELD(UmiBuildDeploymentStepSnapshot, command),
                               FIELD(UmiBuildDeploymentStepSnapshot, working_directory)};
    static const Field af[] = {FIELD(UmiBuildArtifactManifest, artifact_id),
                               FIELD(UmiBuildArtifactManifest, node_id),
                               FIELD(UmiBuildArtifactManifest, target_id),
                               FIELD(UmiBuildArtifactManifest, profile_id),
                               FIELD(UmiBuildArtifactManifest, configuration),
                               FIELD(UmiBuildArtifactManifest, kind),
                               FIELD(UmiBuildArtifactManifest, format),
                               FIELD(UmiBuildArtifactManifest, path),
                               FIELD(UmiBuildArtifactManifest, content_type),
                               FIELD(UmiBuildArtifactManifest, platform),
                               FIELD(UmiBuildArtifactManifest, architecture),
                               FIELD(UmiBuildArtifactManifest, checksum_algorithm),
                               FIELD(UmiBuildArtifactManifest, checksum),
                               FIELD(UmiBuildArtifactManifest, source_revision)};
    static const Field tf[] = {FIELD(UmiBuildDeploymentTargetSnapshot, target_id),
                               FIELD(UmiBuildDeploymentTargetSnapshot, name),
                               FIELD(UmiBuildDeploymentTargetSnapshot, description),
                               FIELD(UmiBuildDeploymentTargetSnapshot, environment),
                               FIELD(UmiBuildDeploymentTargetSnapshot, provider_id),
                               FIELD(UmiBuildDeploymentTargetSnapshot, endpoint),
                               FIELD(UmiBuildDeploymentTargetSnapshot, destination),
                               FIELD(UmiBuildDeploymentTargetSnapshot, working_directory),
                               FIELD(UmiBuildDeploymentTargetSnapshot, health_uri)};
    const Field *fields = pf;
    size_t count = sizeof(pf) / sizeof(pf[0]);
    if (Is(mode, "step-text")) {
      fields = sf;
      count = sizeof(sf) / sizeof(sf[0]);
    }
    if (Is(mode, "artifact-text")) {
      fields = af;
      count = sizeof(af) / sizeof(af[0]);
    }
    if (Is(mode, "target-text")) {
      fields = tf;
      count = sizeof(tf) / sizeof(tf[0]);
    }
    for (size_t i = 0U; i < count; ++i) {
      CHECK(Make(plan, &artifact, &target) == UMI_STATUS_OK);
      unsigned char *object = (unsigned char *)plan;
      if (Is(mode, "step-text"))
        object = (unsigned char *)&plan->steps[0];
      if (Is(mode, "artifact-text"))
        object = (unsigned char *)&artifact;
      if (Is(mode, "target-text"))
        object = (unsigned char *)&target;
      memset(object + fields[i].offset, 'x', fields[i].capacity);
      UmiStatus status =
          Is(mode, "artifact-text") ? umi_build_artifact_manifest_validate(&artifact, NULL, 0U)
          : Is(mode, "target-text") ? umi_build_deployment_target_validate(&target, NULL, 0U)
                                    : umi_build_deployment_plan_validate(plan, NULL, 0U);
      CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
    }
  } else if (Is(mode, "invalid-compose-preserves") || Is(mode, "disabled-target-preserves")) {
    *before = *plan;
    if (Is(mode, "invalid-compose-preserves"))
      artifact.artifact_id[0] = '\0';
    else
      target.flags &= ~(UmiBuildDeploymentTargetFlags)UMI_BUILD_DEPLOYMENT_TARGET_ENABLED;
    CHECK(umi_build_deployment_plan_compose_default(plan, &artifact, &target, 0) != UMI_STATUS_OK);
    CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
  } else if (Is(mode, "add-revokes-approval") || Is(mode, "bind-revokes-approval")) {
    CHECK(umi_build_deployment_plan_approve(plan) == UMI_STATUS_OK);
    CHECK(plan->approved);
    uint64_t revision = plan->revision;
    CHECK((Is(mode, "add-revokes-approval")
               ? umi_build_deployment_plan_add_step(plan, &step)
               : umi_build_deployment_plan_bind(plan, &artifact, &target)) == UMI_STATUS_OK);
    CHECK(!plan->approved && plan->revision == revision + 1U);
  } else if (Is(mode, "compose-revision") || Is(mode, "bind-revision") ||
             Is(mode, "add-revision") || Is(mode, "approve-revision") ||
             Is(mode, "compose-generation")) {
    if (Is(mode, "compose-generation"))
      plan->generation = UINT64_MAX;
    else
      plan->revision = UINT64_MAX;
    *before = *plan;
    UmiStatus status;
    if (Is(mode, "bind-revision"))
      status = umi_build_deployment_plan_bind(plan, &artifact, &target);
    else if (Is(mode, "add-revision"))
      status = umi_build_deployment_plan_add_step(plan, &step);
    else if (Is(mode, "approve-revision"))
      status = umi_build_deployment_plan_approve(plan);
    else
      status = umi_build_deployment_plan_compose_default(plan, &artifact, &target, 0);
    CHECK(status == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
  } else if (Is(mode, "step-out-of-range")) {
    CHECK(umi_build_deployment_plan_step_at(plan, plan->step_count, &step) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(&step, &saved_step, sizeof(step)) == 0);
  } else if (Is(mode, "partial-snapshot")) {
    umi_build_deployment_plan_init(plan, "partial");
    CHECK(umi_build_deployment_plan_snapshot(plan, &snapshot) == UMI_STATUS_OK);
    CHECK(!snapshot.valid && snapshot.step_count == 0U);
  } else if (Is(mode, "source-copy")) {
    CHECK(umi_build_deployment_plan_add_step(plan, &step) == UMI_STATUS_OK);
    step.label[0] = 'X';
    CHECK(strcmp(plan->steps[plan->step_count - 1U].label, "Extra") == 0);
  } else {
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (Is(mode, "valid"))
      expected = UMI_STATUS_OK;
    if (Is(mode, "rollback-install-allowed")) {
      plan->steps[plan->step_count - 1U].kind = UMI_BUILD_DEPLOYMENT_STEP_INSTALL;
      expected = UMI_STATUS_OK;
    }
    if (Is(mode, "plan-metadata"))
      plan->api_version = 0U;
    if (Is(mode, "step-metadata"))
      plan->steps[0].structure_size = 0U;
    if (Is(mode, "artifact-metadata")) {
      artifact.structure_size = 0U;
      CHECK(umi_build_artifact_manifest_validate(&artifact, NULL, 0U) == expected);
      goto done;
    }
    if (Is(mode, "target-metadata")) {
      target.api_version = 0U;
      CHECK(umi_build_deployment_target_validate(&target, NULL, 0U) == expected);
      goto done;
    }
    if (Is(mode, "plan-bool"))
      plan->approved = 2;
    if (Is(mode, "step-bool"))
      plan->steps[0].required = 2;
    if (Is(mode, "plan-strategy"))
      plan->strategy = (UmiBuildDeploymentStrategy)99;
    if (Is(mode, "step-kind"))
      plan->steps[0].kind = (UmiBuildDeploymentStepKind)99;
    if (Is(mode, "step-state"))
      plan->steps[0].state = (UmiBuildDeploymentStepState)99;
    if (Is(mode, "plan-revision-zero"))
      plan->revision = 0U;
    if (Is(mode, "step-revision-zero"))
      plan->steps[0].revision = 0U;
    if (Is(mode, "order"))
      plan->steps[0].order = 1U;
    if (Is(mode, "duplicate")) {
      strcpy(plan->steps[1].step_id, plan->steps[0].step_id);
      expected = UMI_STATUS_ALREADY_EXISTS;
    }
    if (Is(mode, "rollback-gate")) {
      plan->steps[0].rollback_step = 1;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (Is(mode, "optional-gate")) {
      plan->steps[0].required = 0;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (Is(mode, "install-before-validation")) {
      plan->steps[0].kind = UMI_BUILD_DEPLOYMENT_STEP_INSTALL;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (Is(mode, "promote-before-install")) {
      plan->steps[1].kind = UMI_BUILD_DEPLOYMENT_STEP_PROMOTE;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (Is(mode, "missing-gate")) {
      plan->steps[0].kind = UMI_BUILD_DEPLOYMENT_STEP_PREPARE;
      expected = UMI_STATUS_INVALID_STATE;
    }
    if (Is(mode, "invalid-approve-preserves")) {
      plan->artifact_id[0] = '\0';
      *before = *plan;
      CHECK(umi_build_deployment_plan_approve(plan) == UMI_STATUS_INVALID_ARGUMENT);
      CHECK(memcmp(plan, before, sizeof(*plan)) == 0);
      goto done;
    }
    CHECK(umi_build_deployment_plan_validate(plan, NULL, 0U) == expected);
  }
done:
  free(plan);
  free(before);
  return 0;
}
