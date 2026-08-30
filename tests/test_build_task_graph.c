/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_task_graph.c
 *
 * PURPOSE:
 *   Verify readiness, failure blocking and bounded retry transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/task_graph.h"

static void add_task(UmiBuildTaskRegistry *registry,
                     const char *id,
                     UmiBuildTaskKind kind,
                     const char *dependency,
                     uint32_t attempts)
{
    UmiBuildTaskSnapshot task;
    umi_build_task_init(&task, id, id, kind);
    assert(umi_build_task_set_command(&task, id, ".") == UMI_STATUS_OK);
    task.maximum_attempts = attempts;
    if (dependency != NULL) {
        assert(umi_build_task_add_dependency(&task, dependency) ==
               UMI_STATUS_OK);
    }
    assert(umi_build_task_registry_upsert(registry, &task) == UMI_STATUS_OK);
}

int main(void)
{
    UmiBuildTaskRegistry *registry = NULL;
    UmiBuildTaskGraph *graph = NULL;
    UmiBuildTaskGraphNodeSnapshot node;
    UmiBuildTaskGraphSnapshot snapshot;

    assert(umi_build_task_registry_create(&registry) == UMI_STATUS_OK);
    add_task(registry, "configure", UMI_BUILD_TASK_CONFIGURE, NULL, 1U);
    add_task(registry, "build", UMI_BUILD_TASK_BUILD, "configure", 2U);
    add_task(registry, "test", UMI_BUILD_TASK_TEST, "build", 1U);
    assert(umi_build_task_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_build_task_graph_rebuild(graph, registry) == UMI_STATUS_OK);

    assert(umi_build_task_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.task.task_id, "configure") == 0);
    assert(umi_build_task_graph_start(graph, "configure") == UMI_STATUS_OK);
    assert(umi_build_task_graph_complete(graph, "configure", 0) ==
           UMI_STATUS_OK);
    assert(umi_build_task_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.task.task_id, "build") == 0);
    assert(umi_build_task_graph_start(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_task_graph_fail(
               graph, "build", UMI_STATUS_INTERNAL_ERROR, 7) == UMI_STATUS_OK);
    assert(umi_build_task_graph_find(graph, "test", &node) == UMI_STATUS_OK);
    assert(node.state == UMI_BUILD_TASK_BLOCKED);
    assert(umi_build_task_graph_retry(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_task_graph_start(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_task_graph_complete(graph, "build", 0) == UMI_STATUS_OK);
    assert(umi_build_task_graph_find(graph, "test", &node) == UMI_STATUS_OK);
    assert(node.state == UMI_BUILD_TASK_READY);
    assert(umi_build_task_graph_snapshot(graph, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.succeeded_count == 2U);
    assert(snapshot.ready_count == 1U);

    umi_build_task_graph_destroy(graph);
    umi_build_task_registry_destroy(registry);
    return 0;
}
