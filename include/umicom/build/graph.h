/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/graph.h
 *
 * PURPOSE:
 *   Define the dependency-aware configure/build/test/run graph used by Studio,
 *   CI tools and future Umicom applications.  The graph owns lifecycle state;
 *   execution remains behind the build-engine boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_GRAPH_H
#define UMICOM_BUILD_GRAPH_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_GRAPH_API_VERSION 2U

typedef struct UmiBuildGraphNodeSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char node_id[UMI_BUILD_ID_CAPACITY];
    char label[UMI_BUILD_NAME_CAPACITY];
    UmiBuildPhase phase;
    UmiBuildNodeState state;
    uint32_t progress_basis_points;
    uint32_t attempt_count;
    uint32_t maximum_attempts;
    uint32_t timeout_ms;
    int incremental;
    uint64_t input_revision;
    uint64_t completed_input_revision;
    UmiStatus last_status;
    int last_exit_code;
    uint64_t revision;
} UmiBuildGraphNodeSnapshot;

typedef struct UmiBuildGraphSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t node_count;
    size_t dependency_count;
    size_t pending_count;
    size_t ready_count;
    size_t running_count;
    size_t succeeded_count;
    size_t failed_count;
    size_t cancelled_count;
    size_t timed_out_count;
    size_t skipped_count;
    size_t blocked_count;
    uint32_t progress_basis_points;
    uint64_t revision;
} UmiBuildGraphSnapshot;

typedef struct UmiBuildGraph UmiBuildGraph;

void umi_build_graph_node_init(UmiBuildGraphNodeSnapshot *node,
                               const char *node_id,
                               const char *label,
                               UmiBuildPhase phase);
UmiStatus umi_build_graph_create(UmiBuildGraph **out_graph);
void umi_build_graph_destroy(UmiBuildGraph *graph);
UmiStatus umi_build_graph_add_node(UmiBuildGraph *graph,
                                   const UmiBuildGraphNodeSnapshot *node);
UmiStatus umi_build_graph_add_dependency(UmiBuildGraph *graph,
                                         const char *node_id,
                                         const char *depends_on_node_id);
UmiStatus umi_build_graph_find(const UmiBuildGraph *graph,
                               const char *node_id,
                               UmiBuildGraphNodeSnapshot *out_node);
UmiStatus umi_build_graph_at(const UmiBuildGraph *graph,
                             size_t index,
                             UmiBuildGraphNodeSnapshot *out_node);
UmiStatus umi_build_graph_next_ready(UmiBuildGraph *graph,
                                     UmiBuildGraphNodeSnapshot *out_node);
UmiStatus umi_build_graph_start(UmiBuildGraph *graph, const char *node_id);
UmiStatus umi_build_graph_set_progress(UmiBuildGraph *graph,
                                       const char *node_id,
                                       uint32_t progress_basis_points);
UmiStatus umi_build_graph_complete(UmiBuildGraph *graph,
                                   const char *node_id,
                                   int exit_code);
UmiStatus umi_build_graph_fail(UmiBuildGraph *graph,
                               const char *node_id,
                               UmiStatus status,
                               int exit_code);
UmiStatus umi_build_graph_cancel(UmiBuildGraph *graph, const char *node_id);
UmiStatus umi_build_graph_timeout(UmiBuildGraph *graph, const char *node_id);
UmiStatus umi_build_graph_retry(UmiBuildGraph *graph, const char *node_id);
UmiStatus umi_build_graph_invalidate(UmiBuildGraph *graph,
                                     const char *node_id,
                                     uint64_t input_revision);
UmiStatus umi_build_graph_skip_unchanged(UmiBuildGraph *graph,
                                         const char *node_id);
UmiStatus umi_build_graph_refresh(UmiBuildGraph *graph);
UmiStatus umi_build_graph_snapshot(UmiBuildGraph *graph,
                                   UmiBuildGraphSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
