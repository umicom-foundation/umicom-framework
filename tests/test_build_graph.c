/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_graph.c
 * PURPOSE: Verify dependency ordering, progress, blocking and bounded retry.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/graph.h"

static void add(UmiBuildGraph *graph, const char *id, UmiBuildPhase phase,
                uint32_t attempts)
{
    UmiBuildGraphNodeSnapshot node;
    umi_build_graph_node_init(&node, id, id, phase);
    node.maximum_attempts = attempts;
    node.incremental = phase == UMI_BUILD_PHASE_CONFIGURE ||
                       phase == UMI_BUILD_PHASE_BUILD;
    assert(umi_build_graph_add_node(graph, &node) == UMI_STATUS_OK);
}

int main(void)
{
    UmiBuildGraph *graph = NULL;
    UmiBuildGraphNodeSnapshot node;
    UmiBuildGraphSnapshot snapshot;
    assert(umi_build_graph_create(&graph) == UMI_STATUS_OK);
    add(graph, "configure", UMI_BUILD_PHASE_CONFIGURE, 1U);
    add(graph, "build", UMI_BUILD_PHASE_BUILD, 2U);
    add(graph, "test", UMI_BUILD_PHASE_TEST, 1U);
    assert(umi_build_graph_add_dependency(graph, "build", "configure") ==
           UMI_STATUS_OK);
    assert(umi_build_graph_add_dependency(graph, "test", "build") ==
           UMI_STATUS_OK);
    assert(umi_build_graph_add_dependency(graph, "configure", "test") ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.node_id, "configure") == 0);
    assert(umi_build_graph_start(graph, "configure") == UMI_STATUS_OK);
    assert(umi_build_graph_set_progress(graph, "configure", 5000U) ==
           UMI_STATUS_OK);
    assert(umi_build_graph_complete(graph, "configure", 0) == UMI_STATUS_OK);
    assert(umi_build_graph_invalidate(graph, "configure", 1U) == UMI_STATUS_OK);
    assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.node_id, "configure") == 0);
    assert(umi_build_graph_skip_unchanged(graph, "configure") == UMI_STATUS_OK);
    assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.node_id, "build") == 0);
    assert(umi_build_graph_start(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_graph_fail(graph, "build", UMI_STATUS_INTERNAL_ERROR, 1) ==
           UMI_STATUS_OK);
    assert(umi_build_graph_find(graph, "test", &node) == UMI_STATUS_OK);
    assert(node.state == UMI_BUILD_NODE_BLOCKED);
    assert(umi_build_graph_retry(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_graph_start(graph, "build") == UMI_STATUS_OK);
    assert(umi_build_graph_complete(graph, "build", 0) == UMI_STATUS_OK);
    assert(umi_build_graph_snapshot(graph, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.succeeded_count == 1U);
    assert(snapshot.skipped_count == 1U);
    assert(snapshot.ready_count == 1U);
    umi_build_graph_destroy(graph);
    return 0;
}
