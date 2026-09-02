/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/dependency_graph.h
 *
 * PURPOSE:
 *   Build and validate dependency topology including cycle detection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_DEPENDENCY_GRAPH_H
#define UMICOM_REPOSITORY_DEPENDENCY_GRAPH_H
#include "umicom/repository/dependency.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository dependency edge data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryDependencyEdge {
    size_t from_index;
    size_t to_index;
} UmiRepositoryDependencyEdge;
/**
 * Represent the repository dependency graph data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryDependencyGraph {
    UmiRepositoryDependency nodes[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY];
    UmiRepositoryDependencyEdge edges[UMI_REPOSITORY_CONTROL_EDGE_CAPACITY];
    size_t node_count;
    size_t edge_count;
    uint64_t revision;
} UmiRepositoryDependencyGraph;
/**
 * Initialise repository dependency graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_dependency_graph_init(UmiRepositoryDependencyGraph *graph);
/**
 * Provide the repository dependency graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_dependency_graph_add_node(
    UmiRepositoryDependencyGraph *graph,
    const UmiRepositoryDependency *dependency);
/**
 * Provide the repository dependency graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_dependency_graph_add_edge(
    UmiRepositoryDependencyGraph *graph,
    const char *from_id,
    const char *to_id);
/**
 * Provide the repository dependency graph has cycle operation used by this module and its
 * client applications.
 */
int umi_repository_dependency_graph_has_cycle(
    const UmiRepositoryDependencyGraph *graph);
#ifdef __cplusplus
}
#endif
#endif
