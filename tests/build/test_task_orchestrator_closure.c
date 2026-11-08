/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_orchestrator_closure.c
 *
 * PURPOSE:
 *   Verify bounded dependency closure, selected plans and supervised lifecycle policy through
 * public APIs.
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
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }
static UmiStatus Add(UmiBuildTaskRegistry *registry, const char *id, const char *dep,
                     UmiBuildTaskFlags flags) {
  UmiBuildTaskSnapshot task;
  umi_build_task_init(&task, id, id, UMI_BUILD_TASK_BUILD);
  task.maximum_attempts = 2U;
  task.flags = flags;
  UmiStatus s = dep ? umi_build_task_add_dependency(&task, dep) : UMI_STATUS_OK;
  return s == UMI_STATUS_OK ? umi_build_task_registry_upsert(registry, &task) : s;
}
static UmiStatus Complete(UmiBuildTaskOrchestrator *owner, const char *id) {
  UmiStatus s = umi_build_task_orchestrator_start(owner, id);
  return s == UMI_STATUS_OK ? umi_build_task_orchestrator_complete(owner, id, 0) : s;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  UmiBuildTaskRegistry *registry = NULL;
  UmiBuildTaskOrchestrator *owner = NULL;
  CHECK(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
  UmiBuildTaskOperationSnapshot snapshot;
  UmiBuildTaskSnapshot task;
  if (Is(mode, "deep-chain") || Is(mode, "deep-cycle")) {
    for (size_t i = 0U; i < UMI_BUILD_TASK_CAPACITY; ++i) {
      char id[32], dep[32];
      snprintf(id, sizeof id, "task-%zu", i);
      snprintf(dep, sizeof dep, "task-%zu", i ? i - 1U : UMI_BUILD_TASK_CAPACITY - 1U);
      CHECK(Add(registry, id, i || Is(mode, "deep-cycle") ? dep : NULL, UMI_BUILD_TASK_ENABLED) ==
            UMI_STATUS_OK);
    }
    CHECK(umi_build_task_orchestrator_create(registry, 1U, &owner) == UMI_STATUS_OK);
    char last[32];
    snprintf(last, sizeof last, "task-%u", UMI_BUILD_TASK_CAPACITY - 1U);
    UmiStatus expected = Is(mode, "deep-chain") ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    CHECK(umi_build_task_orchestrator_plan(owner, "deep", last) == expected);
    if (expected == UMI_STATUS_OK) {
      CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
            snapshot.planned_count == UMI_BUILD_TASK_CAPACITY);
      CHECK(umi_build_task_orchestrator_begin(owner) == UMI_STATUS_OK);
      CHECK(umi_build_task_orchestrator_next_ready(owner, &task) == UMI_STATUS_OK &&
            strcmp(task.task_id, "task-0") == 0);
    } else
      CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
            snapshot.state == UMI_BUILD_TASK_OPERATION_IDLE && snapshot.planned_count == 0U);
  } else if (Is(mode, "empty-begin")) {
    CHECK(umi_build_task_orchestrator_create(registry, 0U, &owner) == UMI_STATUS_OK);
    CHECK(umi_build_task_orchestrator_plan(owner, "empty", NULL) == UMI_STATUS_OK);
    CHECK(umi_build_task_orchestrator_begin(owner) == UMI_STATUS_OK &&
          umi_build_task_orchestrator_finish(owner) == UMI_STATUS_OK);
    CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
          snapshot.state == UMI_BUILD_TASK_OPERATION_SUCCEEDED && snapshot.maximum_parallel == 1U);
  } else {
    CHECK(Add(registry, "a", NULL, Is(mode, "disabled-dependency") ? 0U : UMI_BUILD_TASK_ENABLED) ==
          UMI_STATUS_OK);
    CHECK(Add(registry, "b", "a", UMI_BUILD_TASK_ENABLED) == UMI_STATUS_OK &&
          Add(registry, "c", "a", UMI_BUILD_TASK_ENABLED) == UMI_STATUS_OK);
    CHECK(Add(registry, "d", "b", Is(mode, "all-enabled") ? 0U : UMI_BUILD_TASK_ENABLED) ==
          UMI_STATUS_OK);
    CHECK(umi_build_task_registry_find(registry, "d", &task) == UMI_STATUS_OK);
    CHECK(umi_build_task_add_dependency(&task, "c") == UMI_STATUS_OK &&
          umi_build_task_registry_upsert(registry, &task) == UMI_STATUS_OK);
    CHECK(umi_build_task_orchestrator_create(registry, 2U, &owner) == UMI_STATUS_OK);
    const char *target = Is(mode, "selected-only") || Is(mode, "source-edit") ? "b"
                         : Is(mode, "all-enabled")                            ? NULL
                                                                              : "d";
    CHECK(umi_build_task_orchestrator_plan(owner, "lesson", target) == UMI_STATUS_OK);
    CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK);
    if (Is(mode, "selected-only"))
      CHECK(snapshot.planned_count == 2U);
    else if (Is(mode, "all-enabled"))
      CHECK(snapshot.planned_count == 3U && snapshot.graph.skipped_count == 0U);
    else if (Is(mode, "diamond-once"))
      CHECK(snapshot.planned_count == 4U && snapshot.graph.dependency_count == 4U);
    else if (Is(mode, "source-edit")) {
      UmiBuildTaskOperationSnapshot before = snapshot;
      CHECK(umi_build_task_registry_remove(registry, "a") == UMI_STATUS_OK);
      CHECK(umi_build_task_orchestrator_plan(owner, "changed", "b") == UMI_STATUS_NOT_FOUND);
      CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
            memcmp(&snapshot, &before, sizeof snapshot) == 0);
    } else if (Is(mode, "graph-address")) {
      UmiBuildTaskGraph *graph = umi_build_task_orchestrator_graph(owner);
      uint64_t generation = snapshot.generation;
      CHECK(umi_build_task_orchestrator_plan(owner, "new", "b") == UMI_STATUS_OK &&
            umi_build_task_orchestrator_graph(owner) == graph);
      CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
            snapshot.generation == generation + 1U && snapshot.planned_count == 2U);
    } else {
      CHECK(umi_build_task_orchestrator_begin(owner) == UMI_STATUS_OK);
      if (Is(mode, "disabled-dependency")) {
        CHECK(umi_build_task_orchestrator_next_ready(owner, &task) == UMI_STATUS_OK &&
              strcmp(task.task_id, "b") == 0);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.graph.skipped_count == 1U);
      } else if (Is(mode, "parallel-bound")) {
        CHECK(Complete(owner, "a") == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_start(owner, "b") == UMI_STATUS_OK &&
              umi_build_task_orchestrator_start(owner, "c") == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_next_ready(owner, &task) == UMI_STATUS_BUSY);
        CHECK(umi_build_task_orchestrator_complete(owner, "b", 0) == UMI_STATUS_OK &&
              umi_build_task_orchestrator_complete(owner, "c", 0) == UMI_STATUS_OK);
        CHECK(Complete(owner, "d") == UMI_STATUS_OK &&
              umi_build_task_orchestrator_finish(owner) == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.active_count == 0U && snapshot.completed_count == 4U &&
              snapshot.state == UMI_BUILD_TASK_OPERATION_SUCCEEDED);
      } else if (Is(mode, "stop-running")) {
        CHECK(umi_build_task_orchestrator_start(owner, "a") == UMI_STATUS_OK &&
              umi_build_task_orchestrator_request_stop(owner) == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.active_count == 1U && snapshot.stop_requested == 1 &&
              snapshot.state == UMI_BUILD_TASK_OPERATION_RUNNING);
        CHECK(umi_build_task_orchestrator_complete(owner, "a", 0) == UMI_STATUS_OK &&
              umi_build_task_orchestrator_finish(owner) == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.state == UMI_BUILD_TASK_OPERATION_CANCELLED &&
              snapshot.graph.cancelled_count == 3U);
      } else if (Is(mode, "retry-failed")) {
        CHECK(umi_build_task_orchestrator_start(owner, "a") == UMI_STATUS_OK &&
              umi_build_task_orchestrator_fail(owner, "a", UMI_STATUS_INTERNAL_ERROR, 7) ==
                  UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.state == UMI_BUILD_TASK_OPERATION_FAILED);
        CHECK(umi_build_task_orchestrator_retry(owner, "a") == UMI_STATUS_OK &&
              Complete(owner, "a") == UMI_STATUS_OK && Complete(owner, "b") == UMI_STATUS_OK &&
              Complete(owner, "c") == UMI_STATUS_OK && Complete(owner, "d") == UMI_STATUS_OK);
        CHECK(umi_build_task_orchestrator_snapshot(owner, &snapshot) == UMI_STATUS_OK &&
              snapshot.state == UMI_BUILD_TASK_OPERATION_SUCCEEDED && snapshot.retry_count == 1U);
      } else
        return 2;
    }
  }
  umi_build_task_orchestrator_destroy(owner);
  umi_build_task_registry_destroy(registry);
  return 0;
}
