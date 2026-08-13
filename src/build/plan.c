/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/plan.c
 * PURPOSE: Implement the canonical dependency-aware build plan factory.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/plan.h"

static UmiStatus add_node(UmiBuildGraph *graph,
                          const UmiBuildExecutionPolicy *policy,
                          const char *node_id,
                          const char *label,
                          UmiBuildPhase phase)
{
    UmiBuildGraphNodeSnapshot node;
    umi_build_graph_node_init(&node, node_id, label, phase);
    node.maximum_attempts = policy->maximum_attempts;
    node.timeout_ms = policy->default_timeout_ms;
    node.incremental =
        (phase == UMI_BUILD_PHASE_CONFIGURE &&
         policy->incremental_configure != 0) ||
        (phase == UMI_BUILD_PHASE_BUILD && policy->incremental_build != 0);
    return umi_build_graph_add_node(graph, &node);
}

UmiStatus umi_build_plan_populate_standard(
    UmiBuildGraph *graph,
    const UmiBuildExecutionPolicy *policy,
    int include_run)
{
    UmiBuildGraphSnapshot snapshot;
    UmiStatus status;
    if (graph == NULL || policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_execution_policy_validate(policy, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_graph_snapshot(graph, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (snapshot.node_count != 0U) return UMI_STATUS_ALREADY_EXISTS;

    status = add_node(graph, policy, "configure", "Configure",
                      UMI_BUILD_PHASE_CONFIGURE);
    if (status == UMI_STATUS_OK)
        status = add_node(graph, policy, "build", "Build",
                          UMI_BUILD_PHASE_BUILD);
    if (status == UMI_STATUS_OK)
        status = add_node(graph, policy, "test", "Test",
                          UMI_BUILD_PHASE_TEST);
    if (status == UMI_STATUS_OK)
        status = umi_build_graph_add_dependency(graph, "build", "configure");
    if (status == UMI_STATUS_OK)
        status = umi_build_graph_add_dependency(graph, "test", "build");
    if (status == UMI_STATUS_OK && include_run != 0) {
        status = add_node(graph, policy, "run", "Run", UMI_BUILD_PHASE_RUN);
        if (status == UMI_STATUS_OK)
            status = umi_build_graph_add_dependency(graph, "run", "build");
    }
    return status;
}
