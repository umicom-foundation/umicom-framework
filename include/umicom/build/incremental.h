/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/incremental.h
 *
 * PURPOSE:
 *   Calculate the smallest deterministic build selection required after source,
 *   configuration, generated-input or dependency revisions change.
 *
 * ARCHITECTURE:
 *   The canonical UmiBuildGraph remains the lifecycle authority.  This module
 *   is a reusable planning projection: callers import bounded node evidence and
 *   dependency edges, evaluate transitive invalidation, then consume an ordered
 *   selection.  No command is executed and no graph state is mutated here.
 *
 * OWNERSHIP:
 *   The planner copies every record. Returned plan items are caller-owned value
 *   snapshots. External synchronisation is required for concurrent mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_INCREMENTAL_H
#define UMICOM_BUILD_INCREMENTAL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/build/graph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_INCREMENTAL_API_VERSION 1U
#define UMI_BUILD_INCREMENTAL_NODE_CAPACITY UMI_BUILD_GRAPH_MAX_NODES
#define UMI_BUILD_INCREMENTAL_EDGE_CAPACITY UMI_BUILD_GRAPH_MAX_DEPENDENCIES

typedef uint32_t UmiBuildIncrementalReasonFlags;
enum {
    UMI_BUILD_INCREMENTAL_REASON_NONE = 0U,
    UMI_BUILD_INCREMENTAL_REASON_EXPLICIT = 1U << 0,
    UMI_BUILD_INCREMENTAL_REASON_INPUT_CHANGED = 1U << 1,
    UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED = 1U << 2,
    UMI_BUILD_INCREMENTAL_REASON_OUTPUT_MISSING = 1U << 3,
    UMI_BUILD_INCREMENTAL_REASON_ALWAYS_RUN = 1U << 4,
    UMI_BUILD_INCREMENTAL_REASON_NON_INCREMENTAL = 1U << 5,
    UMI_BUILD_INCREMENTAL_REASON_FORCED = 1U << 6,
    UMI_BUILD_INCREMENTAL_REASON_ALL = (1U << 7) - 1U
};

typedef struct UmiBuildIncrementalNodeEvidence {
    uint32_t structure_size;
    uint32_t api_version;
    char node_id[UMI_BUILD_ID_CAPACITY];
    char label[UMI_BUILD_NAME_CAPACITY];
    UmiBuildPhase phase;
    uint64_t input_revision;
    uint64_t completed_input_revision;
    UmiBuildIncrementalReasonFlags reason_flags;
    uint32_t estimated_duration_ms;
    int enabled;
    int incremental;
    int output_present;
    uint64_t revision;
} UmiBuildIncrementalNodeEvidence;

typedef struct UmiBuildIncrementalPlanItem {
    uint32_t structure_size;
    uint32_t api_version;
    char node_id[UMI_BUILD_ID_CAPACITY];
    char label[UMI_BUILD_NAME_CAPACITY];
    UmiBuildPhase phase;
    UmiBuildIncrementalReasonFlags reason_flags;
    size_t order;
    size_t dependency_count;
    uint32_t estimated_duration_ms;
    int directly_invalidated;
    int inherited_invalidation;
    int required;
} UmiBuildIncrementalPlanItem;

typedef struct UmiBuildIncrementalSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    size_t node_count;
    size_t dependency_count;
    size_t selected_count;
    size_t direct_count;
    size_t inherited_count;
    size_t unchanged_count;
    size_t disabled_count;
    uint64_t estimated_duration_ms;
    uint64_t source_revision;
    uint64_t revision;
    int evaluated;
    int cyclic;
} UmiBuildIncrementalSnapshot;

typedef struct UmiBuildIncrementalPlanner UmiBuildIncrementalPlanner;

void umi_build_incremental_evidence_init(
    UmiBuildIncrementalNodeEvidence *evidence,
    const char *node_id,
    const char *label,
    UmiBuildPhase phase);
UmiStatus umi_build_incremental_evidence_from_graph_node(
    const UmiBuildGraphNodeSnapshot *node,
    int output_present,
    UmiBuildIncrementalNodeEvidence *out_evidence);
UmiStatus umi_build_incremental_planner_create(
    UmiBuildIncrementalPlanner **out_planner);
void umi_build_incremental_planner_destroy(
    UmiBuildIncrementalPlanner *planner);
void umi_build_incremental_planner_clear(UmiBuildIncrementalPlanner *planner);
UmiStatus umi_build_incremental_planner_upsert(
    UmiBuildIncrementalPlanner *planner,
    const UmiBuildIncrementalNodeEvidence *evidence);
UmiStatus umi_build_incremental_planner_add_dependency(
    UmiBuildIncrementalPlanner *planner,
    const char *node_id,
    const char *depends_on_node_id);
UmiStatus umi_build_incremental_planner_invalidate(
    UmiBuildIncrementalPlanner *planner,
    const char *node_id,
    UmiBuildIncrementalReasonFlags reason_flags);
UmiStatus umi_build_incremental_planner_force_all(
    UmiBuildIncrementalPlanner *planner);
UmiStatus umi_build_incremental_planner_evaluate(
    UmiBuildIncrementalPlanner *planner,
    uint64_t source_revision);
UmiStatus umi_build_incremental_planner_item_at(
    const UmiBuildIncrementalPlanner *planner,
    size_t position,
    UmiBuildIncrementalPlanItem *out_item);
UmiStatus umi_build_incremental_planner_snapshot(
    const UmiBuildIncrementalPlanner *planner,
    UmiBuildIncrementalSnapshot *out_snapshot);
size_t umi_build_incremental_planner_count(
    const UmiBuildIncrementalPlanner *planner);
uint64_t umi_build_incremental_planner_revision(
    const UmiBuildIncrementalPlanner *planner);
const char *umi_build_incremental_reason_text(
    UmiBuildIncrementalReasonFlags reason_flags,
    char *out_text,
    size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_INCREMENTAL_H */
