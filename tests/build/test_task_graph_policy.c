/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_graph_policy.c
 *
 * PURPOSE:
 *   Verify deterministic dependency readiness, failure policy, retry and cancellation through the
 * public graph API.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/task_graph.h"
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
static UmiStatus Add(UmiBuildTaskRegistry *r, const char *id, const char *dep,
                     UmiBuildTaskFlags flags) {
  UmiBuildTaskSnapshot task;
  umi_build_task_init(&task, id, id, UMI_BUILD_TASK_BUILD);
  task.flags = flags;
  task.maximum_attempts = 2U;
  UmiStatus s = dep ? umi_build_task_add_dependency(&task, dep) : UMI_STATUS_OK;
  return s == UMI_STATUS_OK ? umi_build_task_registry_upsert(r, &task) : s;
}
static UmiBuildTaskState State(UmiBuildTaskGraph *g, const char *id) {
  UmiBuildTaskGraphNodeSnapshot n;
  return umi_build_task_graph_find(g, id, &n) == UMI_STATUS_OK ? n.state : (UmiBuildTaskState)99;
}
static UmiStatus Complete(UmiBuildTaskGraph *g, const char *id) {
  UmiStatus s = umi_build_task_graph_start(g, id);
  return s == UMI_STATUS_OK ? umi_build_task_graph_complete(g, id, 0) : s;
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *mode = argv[1];
  UmiBuildTaskRegistry *r = NULL;
  UmiBuildTaskGraph *g = NULL;
  CHECK(umi_build_task_registry_create(&r) == UMI_STATUS_OK &&
        umi_build_task_graph_create(&g) == UMI_STATUS_OK);
  UmiBuildTaskGraphNodeSnapshot node;
  UmiBuildTaskGraphSnapshot snapshot;
  UmiBuildTaskFlags enabled = UMI_BUILD_TASK_ENABLED;
  if (Is(mode, "empty")) {
    CHECK(umi_build_task_graph_rebuild(g, r) == UMI_STATUS_OK);
    CHECK(umi_build_task_graph_next_ready(g, &node) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_build_task_graph_snapshot(g, &snapshot) == UMI_STATUS_OK &&
          snapshot.task_count == 0U && snapshot.progress_basis_points == 0U);
  } else if (Is(mode, "missing") || Is(mode, "cycle") || Is(mode, "disabled-missing") ||
             Is(mode, "disabled-cycle")) {
    int disabled = strncmp(mode, "disabled-", 9U) == 0;
    CHECK(Add(r, "a", strstr(mode, "cycle") ? "b" : "absent", disabled ? 0U : enabled) ==
          UMI_STATUS_OK);
    if (strstr(mode, "cycle"))
      CHECK(Add(r, "b", "a", disabled ? 0U : enabled) == UMI_STATUS_OK);
    UmiStatus expected = disabled            ? UMI_STATUS_OK
                         : Is(mode, "cycle") ? UMI_STATUS_INVALID_STATE
                                             : UMI_STATUS_NOT_FOUND;
    CHECK(umi_build_task_graph_rebuild(g, r) == expected);
    if (disabled)
      CHECK(State(g, "a") == UMI_BUILD_TASK_SKIPPED);
    else
      CHECK(umi_build_task_graph_count(g) == 0U);
  } else {
    if (Is(mode, "reverse-order"))
      CHECK(Add(r, "c", "b", enabled) == UMI_STATUS_OK &&
            Add(r, "b", "a", enabled) == UMI_STATUS_OK &&
            Add(r, "a", NULL, enabled) == UMI_STATUS_OK);
    else {
      CHECK(Add(r, "a", NULL,
                Is(mode, "continue-on-error") ? enabled | UMI_BUILD_TASK_CONTINUE_ON_ERROR
                                              : enabled) == UMI_STATUS_OK);
      CHECK(Add(r, "b", "a", enabled) == UMI_STATUS_OK &&
            Add(r, "c", "b", enabled) == UMI_STATUS_OK);
    }
    if (Is(mode, "diamond")) {
      CHECK(Add(r, "d", "a", enabled) == UMI_STATUS_OK);
      UmiBuildTaskSnapshot t;
      CHECK(umi_build_task_registry_find(r, "c", &t) == UMI_STATUS_OK);
      CHECK(umi_build_task_add_dependency(&t, "d") == UMI_STATUS_OK &&
            umi_build_task_registry_upsert(r, &t) == UMI_STATUS_OK);
    }
    CHECK(umi_build_task_graph_rebuild(g, r) == UMI_STATUS_OK);
    CHECK(State(g, "a") == UMI_BUILD_TASK_READY && State(g, "b") == UMI_BUILD_TASK_PENDING);
    if (Is(mode, "chain-success") || Is(mode, "reverse-order")) {
      for (size_t i = 0U; i < 3U; ++i) {
        const char *id = i == 0U ? "a" : i == 1U ? "b" : "c";
        CHECK(umi_build_task_graph_next_ready(g, &node) == UMI_STATUS_OK &&
              strcmp(node.task.task_id, id) == 0);
        CHECK(Complete(g, id) == UMI_STATUS_OK);
      }
      CHECK(umi_build_task_graph_snapshot(g, &snapshot) == UMI_STATUS_OK &&
            snapshot.succeeded_count == 3U && snapshot.progress_basis_points == 10000U);
    } else if (Is(mode, "diamond")) {
      CHECK(Complete(g, "a") == UMI_STATUS_OK && State(g, "b") == UMI_BUILD_TASK_READY &&
            State(g, "d") == UMI_BUILD_TASK_READY);
      CHECK(Complete(g, "b") == UMI_STATUS_OK && State(g, "c") == UMI_BUILD_TASK_PENDING);
      CHECK(Complete(g, "d") == UMI_STATUS_OK && State(g, "c") == UMI_BUILD_TASK_READY);
    } else if (Is(mode, "skip-unblocks")) {
      CHECK(umi_build_task_graph_skip(g, "a") == UMI_STATUS_OK &&
            State(g, "a") == UMI_BUILD_TASK_SKIPPED && State(g, "b") == UMI_BUILD_TASK_READY);
    } else if (Is(mode, "owned-source")) {
      uint64_t source = umi_build_task_registry_revision(r);
      CHECK(umi_build_task_registry_remove(r, "a") == UMI_STATUS_OK);
      CHECK(State(g, "a") == UMI_BUILD_TASK_READY);
      CHECK(umi_build_task_graph_snapshot(g, &snapshot) == UMI_STATUS_OK &&
            snapshot.source_revision == source);
    } else if (Is(mode, "invalid-state")) {
      uint64_t revision = umi_build_task_graph_revision(g);
      CHECK(umi_build_task_graph_start(g, "b") == UMI_STATUS_INVALID_STATE);
      CHECK(umi_build_task_graph_complete(g, "a", 0) == UMI_STATUS_INVALID_STATE);
      CHECK(umi_build_task_graph_retry(g, "a") == UMI_STATUS_INVALID_STATE);
      CHECK(umi_build_task_graph_start(g, "absent") == UMI_STATUS_NOT_FOUND);
      CHECK(umi_build_task_graph_revision(g) == revision);
    } else {
      CHECK(umi_build_task_graph_start(g, "a") == UMI_STATUS_OK);
      if (Is(mode, "cancel-waiting")) {
        CHECK(umi_build_task_graph_cancel_waiting(g) == UMI_STATUS_OK);
        CHECK(State(g, "a") == UMI_BUILD_TASK_RUNNING &&
              State(g, "b") == UMI_BUILD_TASK_CANCELLED &&
              State(g, "c") == UMI_BUILD_TASK_CANCELLED);
        CHECK(umi_build_task_graph_complete(g, "a", 0) == UMI_STATUS_OK &&
              State(g, "b") == UMI_BUILD_TASK_CANCELLED);
      } else if (Is(mode, "progress-bounds") || Is(mode, "aggregate-progress")) {
        CHECK(umi_build_task_graph_set_progress(g, "a", 6000U) == UMI_STATUS_OK);
        uint64_t revision = umi_build_task_graph_revision(g);
        CHECK(umi_build_task_graph_set_progress(g, "a", 10001U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_build_task_graph_set_progress(g, "b", 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_build_task_graph_revision(g) == revision);
        CHECK(umi_build_task_graph_find(g, "a", &node) == UMI_STATUS_OK &&
              node.progress_basis_points == 6000U);
        CHECK(umi_build_task_graph_snapshot(g, &snapshot) == UMI_STATUS_OK &&
              snapshot.running_count == 1U && snapshot.progress_basis_points == 2000U);
        if (Is(mode, "aggregate-progress")) {
          CHECK(umi_build_task_graph_complete(g, "a", 0) == UMI_STATUS_OK);
          CHECK(umi_build_task_graph_start(g, "b") == UMI_STATUS_OK);
          CHECK(umi_build_task_graph_set_progress(g, "b", 5000U) == UMI_STATUS_OK);
          CHECK(umi_build_task_graph_snapshot(g, &snapshot) == UMI_STATUS_OK &&
                snapshot.succeeded_count == 1U && snapshot.running_count == 1U &&
                snapshot.progress_basis_points == 5000U);
        }
      } else {
        if (Is(mode, "nonzero-complete"))
          CHECK(umi_build_task_graph_complete(g, "a", 7) == UMI_STATUS_OK);
        else
          CHECK(umi_build_task_graph_fail(g, "a", UMI_STATUS_OK, 7) == UMI_STATUS_OK);
        CHECK(umi_build_task_graph_find(g, "a", &node) == UMI_STATUS_OK &&
              node.state == UMI_BUILD_TASK_FAILED && node.last_exit_code == 7 &&
              node.last_status == UMI_STATUS_INTERNAL_ERROR);
        if (Is(mode, "continue-on-error"))
          CHECK(State(g, "b") == UMI_BUILD_TASK_READY && State(g, "c") == UMI_BUILD_TASK_PENDING);
        else {
          CHECK(State(g, "b") == UMI_BUILD_TASK_BLOCKED && State(g, "c") == UMI_BUILD_TASK_BLOCKED);
          if (Is(mode, "retry") || Is(mode, "attempt-limit")) {
            CHECK(umi_build_task_graph_retry(g, "a") == UMI_STATUS_OK &&
                  State(g, "a") == UMI_BUILD_TASK_READY &&
                  State(g, "b") == UMI_BUILD_TASK_PENDING &&
                  State(g, "c") == UMI_BUILD_TASK_PENDING);
            CHECK(umi_build_task_graph_start(g, "a") == UMI_STATUS_OK);
            if (Is(mode, "retry"))
              CHECK(umi_build_task_graph_complete(g, "a", 0) == UMI_STATUS_OK &&
                    State(g, "b") == UMI_BUILD_TASK_READY);
            else {
              CHECK(umi_build_task_graph_fail(g, "a", UMI_STATUS_INTERNAL_ERROR, 8) ==
                    UMI_STATUS_OK);
              uint64_t revision = umi_build_task_graph_revision(g);
              CHECK(umi_build_task_graph_retry(g, "a") == UMI_STATUS_CAPACITY_EXCEEDED &&
                    umi_build_task_graph_revision(g) == revision);
            }
          } else if (Is(mode, "reset-after-failure")) {
            CHECK(umi_build_task_graph_reset(g) == UMI_STATUS_OK);
            CHECK(umi_build_task_graph_find(g, "a", &node) == UMI_STATUS_OK &&
                  node.state == UMI_BUILD_TASK_READY && node.attempt_count == 0U &&
                  node.last_status == UMI_STATUS_OK && node.last_exit_code == 0);
            CHECK(State(g, "b") == UMI_BUILD_TASK_PENDING &&
                  State(g, "c") == UMI_BUILD_TASK_PENDING);
          } else if (!Is(mode, "failure-blocks") && !Is(mode, "nonzero-complete"))
            return 2;
        }
      }
    }
  }
  umi_build_task_graph_destroy(g);
  umi_build_task_registry_destroy(r);
  return 0;
}
