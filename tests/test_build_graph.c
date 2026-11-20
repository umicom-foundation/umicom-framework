/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_graph.c
 *
 * PURPOSE:
 *   Verify dependency ordering, progress, blocking and bounded retry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/build/graph.h"

/*
 * Exercise add and return a clear result when the behaviour no longer matches its
 * contract.
 */
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

static void verify_shared_dependencies(void)
{
    UmiBuildGraph *graph = NULL;
    UmiBuildGraphSnapshot before, after;
    UmiBuildGraphNodeSnapshot node;
    enum { LAYERS = 11, LEAVES = 8 };
    char layers[LAYERS][UMI_BUILD_ID_CAPACITY];
    char leaves[LEAVES][UMI_BUILD_ID_CAPACITY];
    assert(umi_build_graph_create(&graph) == UMI_STATUS_OK);
    for (size_t index = 0; index < LAYERS; ++index) {
        assert(snprintf(layers[index], sizeof layers[index], "combine-%zu", index) > 0);
        add(graph, layers[index], UMI_BUILD_PHASE_BUILD, 1U);
    }
    for (size_t index = 0; index < LEAVES; ++index) {
        assert(snprintf(leaves[index], sizeof leaves[index], "compile-%zu", index) > 0);
        add(graph, leaves[index], UMI_BUILD_PHASE_BUILD, 1U);
    }
    add(graph, "link", UMI_BUILD_PHASE_BUILD, 1U);

    /* Each combine step shares eight compile prerequisites. Put the next
     * combine edge last so a depth-first walk descends before visiting them. */
    for (size_t layer = 0; layer < LAYERS; ++layer) {
        for (size_t leaf = 0; leaf < LEAVES; ++leaf)
            assert(umi_build_graph_add_dependency(graph, layers[layer], leaves[leaf]) == UMI_STATUS_OK);
        if (layer + 1U < LAYERS)
            assert(umi_build_graph_add_dependency(graph, layers[layer], layers[layer + 1U]) == UMI_STATUS_OK);
    }
    assert(umi_build_graph_snapshot(graph, &before) == UMI_STATUS_OK);
    assert(before.node_count == 20U && before.dependency_count == 98U && before.ready_count == 9U);
    assert(umi_build_graph_add_dependency(graph, "link", layers[0]) == UMI_STATUS_OK);
    assert(umi_build_graph_find(graph, "link", &node) == UMI_STATUS_OK && node.state == UMI_BUILD_NODE_PENDING);
    assert(umi_build_graph_snapshot(graph, &before) == UMI_STATUS_OK);
    assert(before.dependency_count == 99U && before.ready_count == LEAVES);

    /* A true back edge reaches link through the shared prerequisite. Reject
     * it without consuming an edge or changing the caller's graph revision. */
    assert(umi_build_graph_add_dependency(graph, leaves[0], "link") == UMI_STATUS_INVALID_STATE);
    assert(umi_build_graph_snapshot(graph, &after) == UMI_STATUS_OK);
    assert(memcmp(&before, &after, sizeof before) == 0);

    /* Run the actual graph transitions to check every prerequisite orders the
     * link stage, including the compile steps that fan into several combines. */
    for (size_t index = 0; index < LEAVES; ++index) {
        assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
        assert(strcmp(node.node_id, leaves[index]) == 0);
        assert(umi_build_graph_start(graph, node.node_id) == UMI_STATUS_OK);
        assert(umi_build_graph_complete(graph, node.node_id, 0) == UMI_STATUS_OK);
    }
    for (size_t remaining = LAYERS; remaining > 0; --remaining) {
        assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
        assert(strcmp(node.node_id, layers[remaining - 1U]) == 0);
        assert(umi_build_graph_start(graph, node.node_id) == UMI_STATUS_OK);
        assert(umi_build_graph_complete(graph, node.node_id, 0) == UMI_STATUS_OK);
    }
    assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK && strcmp(node.node_id, "link") == 0);
    assert(umi_build_graph_start(graph, "link") == UMI_STATUS_OK);
    assert(umi_build_graph_complete(graph, "link", 0) == UMI_STATUS_OK);
    assert(umi_build_graph_snapshot(graph, &after) == UMI_STATUS_OK && after.succeeded_count == 20U);
    assert(after.ready_count == 0U && after.progress_basis_points == 10000U);
    umi_build_graph_destroy(graph);
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    verify_shared_dependencies();
    return 0;
}
