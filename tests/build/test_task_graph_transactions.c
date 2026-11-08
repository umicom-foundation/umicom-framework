/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_task_graph_transactions.c
 *
 * PURPOSE:
 *   Verify atomic graph replacement, runtime refusal and dependency observation revisions.
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
#include <stdlib.h>
#include <string.h>
static int refuse_allocation;
static void *GraphCalloc(size_t count, size_t size) {
  if (refuse_allocation)
    return NULL;
  return calloc(count, size);
}
/* Reach private revision and allocation boundaries without adding test-only
 * mutators to the public SDK or replacing the graph implementation. */
#define calloc GraphCalloc
#include "../../src/build/task_graph.c"
#undef calloc
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
  UmiBuildTaskGraph *graph = NULL;
  CHECK(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
  CHECK(Add(registry, "a", NULL) == UMI_STATUS_OK && Add(registry, "b", "a") == UMI_STATUS_OK &&
        Add(registry, "c", "b") == UMI_STATUS_OK);
  CHECK(umi_build_task_graph_create(&graph) == UMI_STATUS_OK);
  CHECK(umi_build_task_graph_rebuild(graph, registry) == UMI_STATUS_OK);
  UmiBuildTaskGraph *before = malloc(sizeof *before);
  CHECK(before != NULL);
  UmiStatus status = UMI_STATUS_OK, expected = UMI_STATUS_CAPACITY_EXCEEDED;
  if (strncmp(mode, "rebuild-", 8U) == 0) {
    if (Is(mode, "rebuild-missing")) {
      CHECK(Add(registry, "b", "absent") == UMI_STATUS_OK);
      expected = UMI_STATUS_NOT_FOUND;
    } else if (Is(mode, "rebuild-cycle")) {
      CHECK(Add(registry, "a", "c") == UMI_STATUS_OK);
      expected = UMI_STATUS_INVALID_STATE;
    } else if (Is(mode, "rebuild-allocation")) {
      refuse_allocation = 1;
      expected = UMI_STATUS_OUT_OF_MEMORY;
    } else if (Is(mode, "rebuild-revision"))
      graph->revision = UINT64_MAX - 1U;
    else
      return 2;
    *before = *graph;
    status = umi_build_task_graph_rebuild(graph, registry);
    refuse_allocation = 0;
  } else if (Is(mode, "dependency-count-revision")) {
    CHECK(Add(registry, "independent", NULL) == UMI_STATUS_OK);
    UmiBuildTaskSnapshot t;
    CHECK(umi_build_task_registry_find(registry, "b", &t) == UMI_STATUS_OK);
    CHECK(umi_build_task_add_dependency(&t, "independent") == UMI_STATUS_OK &&
          umi_build_task_registry_upsert(registry, &t) == UMI_STATUS_OK);
    CHECK(umi_build_task_graph_rebuild(graph, registry) == UMI_STATUS_OK);
    const uint64_t revision = graph->nodes[1].revision;
    CHECK(umi_build_task_graph_start(graph, "a") == UMI_STATUS_OK &&
          umi_build_task_graph_complete(graph, "a", 0) == UMI_STATUS_OK);
    CHECK(graph->nodes[1].state == UMI_BUILD_TASK_PENDING &&
          graph->nodes[1].satisfied_dependency_count == 1U &&
          graph->nodes[1].revision == revision + 1U);
    free(before);
    umi_build_task_graph_destroy(graph);
    umi_build_task_registry_destroy(registry);
    return 0;
  } else if (Is(mode, "unchanged-terminal-limit")) {
    CHECK(umi_build_task_graph_start(graph, "a") == UMI_STATUS_OK &&
          umi_build_task_graph_complete(graph, "a", 0) == UMI_STATUS_OK);
    graph->nodes[0].revision = UINT64_MAX;
    CHECK(umi_build_task_graph_start(graph, "b") == UMI_STATUS_OK);
    CHECK(graph->nodes[0].revision == UINT64_MAX &&
          graph->nodes[0].state == UMI_BUILD_TASK_SUCCEEDED);
    free(before);
    umi_build_task_graph_destroy(graph);
    umi_build_task_registry_destroy(registry);
    return 0;
  } else {
    const int finish = strncmp(mode, "complete-", 9U) == 0 || strncmp(mode, "fail-", 5U) == 0 ||
                       Is(mode, "finish-missing");
    const int progress = strncmp(mode, "progress-", 9U) == 0;
    const int retry = strncmp(mode, "retry-", 6U) == 0;
    if (finish || progress || retry)
      CHECK(umi_build_task_graph_start(graph, "a") == UMI_STATUS_OK);
    if (retry)
      CHECK(umi_build_task_graph_fail(graph, "a", UMI_STATUS_INTERNAL_ERROR, 7) == UMI_STATUS_OK);
    if (Is(mode, "refresh-missing") || Is(mode, "finish-missing")) {
      strcpy(graph->nodes[1].task.dependencies[0], "missing");
      expected = UMI_STATUS_NOT_FOUND;
    } else if (Is(mode, "complete-downstream"))
      graph->nodes[1].revision = UINT64_MAX;
    else if (strstr(mode, "-graph") != NULL)
      graph->revision = (finish || retry || strncmp(mode, "reset-", 6U) == 0 ||
                         strncmp(mode, "cancel-", 7U) == 0 || strncmp(mode, "skip-", 5U) == 0)
                            ? UINT64_MAX - 1U
                            : UINT64_MAX;
    else if (strstr(mode, "-node") != NULL) {
      size_t index = Is(mode, "cancel-waiting-node") ? 2U : 0U;
      graph->nodes[index].revision = UINT64_MAX;
      if (Is(mode, "refresh-node"))
        graph->nodes[0].state = UMI_BUILD_TASK_PENDING;
    } else
      return 2;
    /* cancel_waiting reserves one graph token, unlike cancel + refresh. */
    if (Is(mode, "cancel-waiting-graph"))
      graph->revision = UINT64_MAX;
    *before = *graph;
    if (strncmp(mode, "reset-", 6U) == 0)
      status = umi_build_task_graph_reset(graph);
    else if (strncmp(mode, "refresh-", 8U) == 0)
      status = umi_build_task_graph_refresh(graph);
    else if (strncmp(mode, "start-", 6U) == 0)
      status = umi_build_task_graph_start(graph, "a");
    else if (progress)
      status = umi_build_task_graph_set_progress(graph, "a", 1234U);
    else if (strncmp(mode, "complete-", 9U) == 0 || Is(mode, "finish-missing"))
      status = umi_build_task_graph_complete(graph, "a", 0);
    else if (strncmp(mode, "fail-", 5U) == 0)
      status = umi_build_task_graph_fail(graph, "a", UMI_STATUS_INTERNAL_ERROR, 7);
    else if (strncmp(mode, "cancel-waiting-", 15U) == 0)
      status = umi_build_task_graph_cancel_waiting(graph);
    else if (strncmp(mode, "cancel-", 7U) == 0)
      status = umi_build_task_graph_cancel(graph, "a");
    else if (retry)
      status = umi_build_task_graph_retry(graph, "a");
    else if (strncmp(mode, "skip-", 5U) == 0)
      status = umi_build_task_graph_skip(graph, "a");
    else
      return 2;
  }
  CHECK(status == expected && memcmp(before, graph, sizeof *graph) == 0);
  free(before);
  umi_build_task_graph_destroy(graph);
  umi_build_task_registry_destroy(registry);
  return 0;
}
