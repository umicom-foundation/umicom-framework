/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_orchestrator_transactions.c
 *
 * PURPOSE:
 *   Verify plan preservation, owner identity refusal and status-gated nested graph mutations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/task_orchestrator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Reach private owner and child identity limits without exposing production
 * mutation hooks. The actual task registry remains linked from the SDK target. */
#include "../../src/build/task_graph.c"
static int refuse_registry;
static UmiStatus TestRegistryCreate(UmiBuildTaskRegistry **out) {
  if (refuse_registry) {
    *out = NULL;
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  return umi_build_task_registry_create(out);
}
#define umi_build_task_registry_create TestRegistryCreate
#include "../../src/build/task_orchestrator.c"
#undef umi_build_task_registry_create
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static UmiStatus Add(UmiBuildTaskRegistry *r, const char *id, const char *dep) {
  UmiBuildTaskSnapshot task;
  umi_build_task_init(&task, id, id, UMI_BUILD_TASK_BUILD);
  task.maximum_attempts = 2U;
  UmiStatus status = dep ? umi_build_task_add_dependency(&task, dep) : UMI_STATUS_OK;
  return status == UMI_STATUS_OK ? umi_build_task_registry_upsert(r, &task) : status;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  UmiBuildTaskRegistry *registry = NULL;
  UmiBuildTaskOrchestrator *owner = NULL;
  CHECK(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
  CHECK(Add(registry, "a", NULL) == UMI_STATUS_OK && Add(registry, "b", "a") == UMI_STATUS_OK);
  CHECK(umi_build_task_orchestrator_create(registry, 1U, &owner) == UMI_STATUS_OK);
  if (!Is(mode, "finish-idle"))
    CHECK(umi_build_task_orchestrator_plan(owner, "original", "b") == UMI_STATUS_OK);
  int planning = strncmp(mode, "plan-", 5U) == 0 || Is(mode, "operation-too-long") ||
                 Is(mode, "requested-too-long");
  if (Is(mode, "plan-alias")) {
    UmiBuildTaskGraph *original = owner->graph;
    uint64_t generation = owner->generation;
    CHECK(umi_build_task_orchestrator_plan(owner, owner->operation_id, owner->requested_task_id) ==
          UMI_STATUS_OK);
    CHECK(strcmp(owner->operation_id, "original") == 0 &&
          strcmp(owner->requested_task_id, "b") == 0 && owner->graph == original &&
          owner->generation == generation + 1U);
    umi_build_task_orchestrator_destroy(owner);
    umi_build_task_registry_destroy(registry);
    return 0;
  }
  if ((!planning && !Is(mode, "begin-revision") && !Is(mode, "finish-idle") &&
       !Is(mode, "finish-planned")) ||
      Is(mode, "plan-running"))
    CHECK(umi_build_task_orchestrator_begin(owner) == UMI_STATUS_OK);
  int running = strncmp(mode, "progress-", 9U) == 0 || strncmp(mode, "complete-", 9U) == 0 ||
                strncmp(mode, "fail-", 5U) == 0 || strncmp(mode, "retry-", 6U) == 0 ||
                Is(mode, "finish-active");
  if (running)
    CHECK(umi_build_task_orchestrator_start(owner, "a") == UMI_STATUS_OK);
  if (strncmp(mode, "retry-", 6U) == 0)
    CHECK(umi_build_task_orchestrator_fail(owner, "a", UMI_STATUS_INTERNAL_ERROR, 7) ==
          UMI_STATUS_OK);
  if (Is(mode, "finish-revision")) {
    CHECK(umi_build_task_orchestrator_request_stop(owner) == UMI_STATUS_OK);
  }
  UmiStatus expected = UMI_STATUS_CAPACITY_EXCEEDED;
  if (Is(mode, "plan-missing")) {
    CHECK(Add(registry, "b", "absent") == UMI_STATUS_OK);
    expected = UMI_STATUS_NOT_FOUND;
  } else if (Is(mode, "plan-cycle")) {
    CHECK(Add(registry, "a", "b") == UMI_STATUS_OK);
    expected = UMI_STATUS_INVALID_STATE;
  } else if (Is(mode, "plan-allocation")) {
    refuse_registry = 1;
    expected = UMI_STATUS_OUT_OF_MEMORY;
  } else if (Is(mode, "plan-generation"))
    owner->generation = UINT64_MAX;
  else if (Is(mode, "retry-count"))
    owner->retry_count = SIZE_MAX;
  else if (strstr(mode, "-revision"))
    owner->revision = UINT64_MAX;
  else if (Is(mode, "plan-graph") || Is(mode, "complete-graph"))
    owner->graph->revision = UINT64_MAX - 1U;
  else if (Is(mode, "stop-graph"))
    owner->graph->revision = UINT64_MAX;
  else if (Is(mode, "finish-active") || Is(mode, "plan-running"))
    expected = UMI_STATUS_BUSY;
  else if (Is(mode, "finish-idle") || Is(mode, "finish-planned") || Is(mode, "finish-waiting"))
    expected = UMI_STATUS_INVALID_STATE;
  else if (!Is(mode, "operation-too-long") && !Is(mode, "requested-too-long"))
    return 2;
  UmiBuildTaskOrchestrator saved = *owner;
  UmiBuildTaskGraph *graph = malloc(sizeof *graph);
  CHECK(graph != NULL);
  *graph = *owner->graph;
  size_t count = umi_build_task_registry_count(owner->planned_registry);
  uint64_t revision = umi_build_task_registry_revision(owner->planned_registry);
  char oversized[UMI_BUILD_ID_CAPACITY + 1U];
  memset(oversized, 'x', sizeof oversized - 1U);
  oversized[sizeof oversized - 1U] = '\0';
  UmiStatus status;
  if (planning)
    status = umi_build_task_orchestrator_plan(
        owner, Is(mode, "operation-too-long") ? oversized : "replacement",
        Is(mode, "requested-too-long") ? oversized : "b");
  else if (Is(mode, "begin-revision"))
    status = umi_build_task_orchestrator_begin(owner);
  else if (Is(mode, "start-revision"))
    status = umi_build_task_orchestrator_start(owner, "a");
  else if (Is(mode, "progress-revision"))
    status = umi_build_task_orchestrator_set_progress(owner, "a", 5000U);
  else if (strncmp(mode, "complete-", 9U) == 0)
    status = umi_build_task_orchestrator_complete(owner, "a", 0);
  else if (Is(mode, "fail-revision"))
    status = umi_build_task_orchestrator_fail(owner, "a", UMI_STATUS_INTERNAL_ERROR, 9);
  else if (strncmp(mode, "retry-", 6U) == 0)
    status = umi_build_task_orchestrator_retry(owner, "a");
  else if (strncmp(mode, "stop-", 5U) == 0)
    status = umi_build_task_orchestrator_request_stop(owner);
  else if (strncmp(mode, "finish-", 7U) == 0)
    status = umi_build_task_orchestrator_finish(owner);
  else
    return 2;
  refuse_registry = 0;
  CHECK(status == expected && memcmp(&saved, owner, sizeof saved) == 0 &&
        memcmp(graph, owner->graph, sizeof *graph) == 0);
  CHECK(umi_build_task_registry_count(owner->planned_registry) == count &&
        umi_build_task_registry_revision(owner->planned_registry) == revision);
  free(graph);
  umi_build_task_orchestrator_destroy(owner);
  umi_build_task_registry_destroy(registry);
  return 0;
}
