/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_plan.c
 * PURPOSE: Verify the canonical configure/build/test/run dependency plan.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/plan.h"

int main(void)
{
    UmiBuildExecutionPolicy policy;
    UmiBuildGraph *graph = NULL;
    UmiBuildGraphSnapshot snapshot;
    UmiBuildGraphNodeSnapshot node;
    umi_build_execution_policy_init(&policy);
    policy.default_timeout_ms = 90000U;
    assert(umi_build_graph_create(&graph) == UMI_STATUS_OK);
    assert(umi_build_plan_populate_standard(graph, &policy, 1) == UMI_STATUS_OK);
    assert(umi_build_graph_snapshot(graph, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.node_count == 4U);
    assert(snapshot.dependency_count == 3U);
    assert(umi_build_graph_next_ready(graph, &node) == UMI_STATUS_OK);
    assert(strcmp(node.node_id, "configure") == 0);
    assert(node.timeout_ms == 90000U);
    assert(node.maximum_attempts == 2U);
    assert(node.incremental == 1);
    assert(umi_build_plan_populate_standard(graph, &policy, 0) ==
           UMI_STATUS_ALREADY_EXISTS);
    umi_build_graph_destroy(graph);
    return 0;
}
