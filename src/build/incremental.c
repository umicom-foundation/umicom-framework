/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/incremental.c
 *
 * PURPOSE:
 *   Implement revision-aware transitive build invalidation and deterministic
 *   dependency-first ordering without mutating the canonical build graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/incremental.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct IncrementalEdge {
    size_t node_index;
    size_t dependency_index;
} IncrementalEdge;

struct UmiBuildIncrementalPlanner {
    UmiBuildIncrementalNodeEvidence nodes[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    IncrementalEdge edges[UMI_BUILD_INCREMENTAL_EDGE_CAPACITY];
    UmiBuildIncrementalPlanItem plan[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    size_t node_count;
    size_t edge_count;
    size_t plan_count;
    uint64_t source_revision;
    uint64_t revision;
    int evaluated;
    int cyclic;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static size_t find_node(const UmiBuildIncrementalPlanner *planner,
                        const char *node_id)
{
    size_t index;
    if (planner == NULL || node_id == NULL) return SIZE_MAX;
    for (index = 0U; index < planner->node_count; ++index) {
        if (strcmp(planner->nodes[index].node_id, node_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int edge_exists(const UmiBuildIncrementalPlanner *planner,
                       size_t node_index,
                       size_t dependency_index)
{
    size_t index;
    for (index = 0U; index < planner->edge_count; ++index) {
        if (planner->edges[index].node_index == node_index &&
            planner->edges[index].dependency_index == dependency_index) {
            return 1;
        }
    }
    return 0;
}

void umi_build_incremental_evidence_init(
    UmiBuildIncrementalNodeEvidence *evidence,
    const char *node_id,
    const char *label,
    UmiBuildPhase phase)
{
    if (evidence == NULL) return;
    (void)memset(evidence, 0, sizeof(*evidence));
    evidence->structure_size = (uint32_t)sizeof(*evidence);
    evidence->api_version = UMI_BUILD_INCREMENTAL_API_VERSION;
    copy_text(evidence->node_id, sizeof(evidence->node_id), node_id);
    copy_text(evidence->label, sizeof(evidence->label), label);
    evidence->phase = phase;
    evidence->enabled = 1;
    evidence->incremental = 1;
    evidence->output_present = 1;
    evidence->revision = 1U;
}

UmiStatus umi_build_incremental_evidence_from_graph_node(
    const UmiBuildGraphNodeSnapshot *node,
    int output_present,
    UmiBuildIncrementalNodeEvidence *out_evidence)
{
    if (node == NULL || out_evidence == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_build_incremental_evidence_init(
        out_evidence, node->node_id, node->label, node->phase);
    out_evidence->input_revision = node->input_revision;
    out_evidence->completed_input_revision = node->completed_input_revision;
    out_evidence->incremental = node->incremental != 0;
    out_evidence->output_present = output_present != 0;
    out_evidence->enabled = node->state != UMI_BUILD_NODE_SKIPPED;
    out_evidence->revision = node->revision;
    if (node->state == UMI_BUILD_NODE_FAILED ||
        node->state == UMI_BUILD_NODE_CANCELLED ||
        node->state == UMI_BUILD_NODE_TIMED_OUT) {
        out_evidence->reason_flags |= UMI_BUILD_INCREMENTAL_REASON_EXPLICIT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_create(
    UmiBuildIncrementalPlanner **out_planner)
{
    UmiBuildIncrementalPlanner *planner;
    if (out_planner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_planner = NULL;
    planner = (UmiBuildIncrementalPlanner *)calloc(1U, sizeof(*planner));
    if (planner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    planner->revision = 1U;
    *out_planner = planner;
    return UMI_STATUS_OK;
}

void umi_build_incremental_planner_destroy(
    UmiBuildIncrementalPlanner *planner)
{
    free(planner);
}

void umi_build_incremental_planner_clear(UmiBuildIncrementalPlanner *planner)
{
    if (planner == NULL) return;
    (void)memset(planner->nodes, 0, sizeof(planner->nodes));
    (void)memset(planner->edges, 0, sizeof(planner->edges));
    (void)memset(planner->plan, 0, sizeof(planner->plan));
    planner->node_count = 0U;
    planner->edge_count = 0U;
    planner->plan_count = 0U;
    planner->source_revision = 0U;
    planner->evaluated = 0;
    planner->cyclic = 0;
    planner->revision += 1U;
}

static UmiStatus validate_evidence(
    const UmiBuildIncrementalNodeEvidence *evidence)
{
    if (evidence == NULL || evidence->node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (evidence->phase < UMI_BUILD_PHASE_CONFIGURE ||
        evidence->phase > UMI_BUILD_PHASE_INSTALL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if ((evidence->reason_flags &
         ~(UmiBuildIncrementalReasonFlags)UMI_BUILD_INCREMENTAL_REASON_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_upsert(
    UmiBuildIncrementalPlanner *planner,
    const UmiBuildIncrementalNodeEvidence *evidence)
{
    UmiBuildIncrementalNodeEvidence copy;
    size_t index;
    UmiStatus status;
    if (planner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = validate_evidence(evidence);
    if (status != UMI_STATUS_OK) return status;
    copy = *evidence;
    copy.structure_size = (uint32_t)sizeof(copy);
    copy.api_version = UMI_BUILD_INCREMENTAL_API_VERSION;
    if (copy.revision == 0U) copy.revision = 1U;
    index = find_node(planner, copy.node_id);
    if (index == SIZE_MAX) {
        if (planner->node_count >= UMI_BUILD_INCREMENTAL_NODE_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        planner->nodes[planner->node_count++] = copy;
    } else {
        copy.revision = planner->nodes[index].revision + 1U;
        planner->nodes[index] = copy;
    }
    planner->evaluated = 0;
    planner->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_add_dependency(
    UmiBuildIncrementalPlanner *planner,
    const char *node_id,
    const char *depends_on_node_id)
{
    size_t node_index;
    size_t dependency_index;
    if (planner == NULL || node_id == NULL || depends_on_node_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    node_index = find_node(planner, node_id);
    dependency_index = find_node(planner, depends_on_node_id);
    if (node_index == SIZE_MAX || dependency_index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (node_index == dependency_index) return UMI_STATUS_INVALID_ARGUMENT;
    if (edge_exists(planner, node_index, dependency_index)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (planner->edge_count >= UMI_BUILD_INCREMENTAL_EDGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    planner->edges[planner->edge_count].node_index = node_index;
    planner->edges[planner->edge_count].dependency_index = dependency_index;
    planner->edge_count += 1U;
    planner->evaluated = 0;
    planner->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_invalidate(
    UmiBuildIncrementalPlanner *planner,
    const char *node_id,
    UmiBuildIncrementalReasonFlags reason_flags)
{
    size_t index;
    if (planner == NULL || node_id == NULL || reason_flags == 0U ||
        (reason_flags &
         ~(UmiBuildIncrementalReasonFlags)UMI_BUILD_INCREMENTAL_REASON_ALL) != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(planner, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    planner->nodes[index].reason_flags |= reason_flags;
    planner->nodes[index].revision += 1U;
    planner->evaluated = 0;
    planner->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_force_all(
    UmiBuildIncrementalPlanner *planner)
{
    size_t index;
    if (planner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < planner->node_count; ++index) {
        if (planner->nodes[index].enabled) {
            planner->nodes[index].reason_flags |=
                UMI_BUILD_INCREMENTAL_REASON_FORCED;
            planner->nodes[index].revision += 1U;
        }
    }
    planner->evaluated = 0;
    planner->revision += 1U;
    return UMI_STATUS_OK;
}

static UmiBuildIncrementalReasonFlags direct_reasons(
    const UmiBuildIncrementalNodeEvidence *node)
{
    UmiBuildIncrementalReasonFlags reasons = node->reason_flags;
    reasons &=
        ~(UmiBuildIncrementalReasonFlags)
            UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED;
    if (!node->incremental) {
        reasons |= UMI_BUILD_INCREMENTAL_REASON_NON_INCREMENTAL;
    }
    if (!node->output_present) {
        reasons |= UMI_BUILD_INCREMENTAL_REASON_OUTPUT_MISSING;
    }
    if (node->input_revision != node->completed_input_revision) {
        reasons |= UMI_BUILD_INCREMENTAL_REASON_INPUT_CHANGED;
    }
    return reasons;
}

UmiStatus umi_build_incremental_planner_evaluate(
    UmiBuildIncrementalPlanner *planner,
    uint64_t source_revision)
{
    UmiBuildIncrementalReasonFlags reasons[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    unsigned char selected[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    size_t indegree[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    size_t queue[UMI_BUILD_INCREMENTAL_NODE_CAPACITY];
    size_t queue_begin = 0U;
    size_t queue_end = 0U;
    size_t produced = 0U;
    size_t index;
    int changed;

    if (planner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(reasons, 0, sizeof(reasons));
    (void)memset(selected, 0, sizeof(selected));
    (void)memset(indegree, 0, sizeof(indegree));
    planner->plan_count = 0U;
    planner->cyclic = 0;

    for (index = 0U; index < planner->node_count; ++index) {
        if (!planner->nodes[index].enabled) continue;
        reasons[index] = direct_reasons(&planner->nodes[index]);
        selected[index] = reasons[index] != UMI_BUILD_INCREMENTAL_REASON_NONE;
    }

    do {
        changed = 0;
        for (index = 0U; index < planner->edge_count; ++index) {
            const size_t consumer = planner->edges[index].node_index;
            const size_t dependency = planner->edges[index].dependency_index;
            if (!planner->nodes[consumer].enabled ||
                !planner->nodes[dependency].enabled) {
                continue;
            }
            if (selected[dependency] != 0U && selected[consumer] == 0U) {
                selected[consumer] = 1U;
                reasons[consumer] |=
                    UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED;
                changed = 1;
            } else if (selected[dependency] != 0U) {
                reasons[consumer] |=
                    UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED;
            }
        }
    } while (changed != 0);

    for (index = 0U; index < planner->edge_count; ++index) {
        const size_t consumer = planner->edges[index].node_index;
        const size_t dependency = planner->edges[index].dependency_index;
        if (selected[consumer] != 0U && selected[dependency] != 0U) {
            indegree[consumer] += 1U;
        }
    }
    for (index = 0U; index < planner->node_count; ++index) {
        if (selected[index] != 0U && indegree[index] == 0U) {
            queue[queue_end++] = index;
        }
    }

    while (queue_begin < queue_end) {
        const size_t node_index = queue[queue_begin++];
        UmiBuildIncrementalPlanItem *item = &planner->plan[produced];
        size_t edge_index;
        size_t dependency_count = 0U;
        (void)memset(item, 0, sizeof(*item));
        item->structure_size = (uint32_t)sizeof(*item);
        item->api_version = UMI_BUILD_INCREMENTAL_API_VERSION;
        copy_text(item->node_id, sizeof(item->node_id),
                  planner->nodes[node_index].node_id);
        copy_text(item->label, sizeof(item->label),
                  planner->nodes[node_index].label);
        item->phase = planner->nodes[node_index].phase;
        item->reason_flags = reasons[node_index];
        item->order = produced;
        item->estimated_duration_ms =
            planner->nodes[node_index].estimated_duration_ms;
        item->directly_invalidated =
            (reasons[node_index] &
             ~(UmiBuildIncrementalReasonFlags)
                 UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED) != 0U;
        item->inherited_invalidation =
            (reasons[node_index] &
             UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED) != 0U;
        item->required = 1;
        for (edge_index = 0U; edge_index < planner->edge_count; ++edge_index) {
            if (planner->edges[edge_index].node_index == node_index &&
                selected[planner->edges[edge_index].dependency_index] != 0U) {
                dependency_count += 1U;
            }
        }
        item->dependency_count = dependency_count;
        produced += 1U;

        for (edge_index = 0U; edge_index < planner->edge_count; ++edge_index) {
            if (planner->edges[edge_index].dependency_index == node_index) {
                const size_t consumer = planner->edges[edge_index].node_index;
                if (selected[consumer] == 0U) continue;
                if (indegree[consumer] > 0U) indegree[consumer] -= 1U;
                if (indegree[consumer] == 0U) queue[queue_end++] = consumer;
            }
        }
    }

    for (index = 0U; index < planner->node_count; ++index) {
        if (selected[index] != 0U && indegree[index] != 0U) {
            planner->cyclic = 1;
            planner->plan_count = 0U;
            planner->source_revision = source_revision;
            planner->evaluated = 1;
            planner->revision += 1U;
            return UMI_STATUS_INVALID_STATE;
        }
    }

    planner->plan_count = produced;
    planner->source_revision = source_revision;
    planner->evaluated = 1;
    planner->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_item_at(
    const UmiBuildIncrementalPlanner *planner,
    size_t position,
    UmiBuildIncrementalPlanItem *out_item)
{
    if (planner == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (!planner->evaluated) return UMI_STATUS_INVALID_STATE;
    if (position >= planner->plan_count) return UMI_STATUS_NOT_FOUND;
    *out_item = planner->plan[position];
    return UMI_STATUS_OK;
}

UmiStatus umi_build_incremental_planner_snapshot(
    const UmiBuildIncrementalPlanner *planner,
    UmiBuildIncrementalSnapshot *out_snapshot)
{
    size_t index;
    if (planner == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_INCREMENTAL_API_VERSION;
    out_snapshot->node_count = planner->node_count;
    out_snapshot->dependency_count = planner->edge_count;
    out_snapshot->selected_count = planner->plan_count;
    out_snapshot->source_revision = planner->source_revision;
    out_snapshot->revision = planner->revision;
    out_snapshot->evaluated = planner->evaluated;
    out_snapshot->cyclic = planner->cyclic;
    for (index = 0U; index < planner->node_count; ++index) {
        if (!planner->nodes[index].enabled) out_snapshot->disabled_count += 1U;
    }
    if (planner->evaluated) {
        for (index = 0U; index < planner->plan_count; ++index) {
            if (planner->plan[index].directly_invalidated) {
                out_snapshot->direct_count += 1U;
            } else if (planner->plan[index].inherited_invalidation) {
                out_snapshot->inherited_count += 1U;
            }
            out_snapshot->estimated_duration_ms +=
                planner->plan[index].estimated_duration_ms;
        }
        out_snapshot->unchanged_count =
            planner->node_count - planner->plan_count -
            out_snapshot->disabled_count;
    }
    return UMI_STATUS_OK;
}

size_t umi_build_incremental_planner_count(
    const UmiBuildIncrementalPlanner *planner)
{
    return planner != NULL ? planner->plan_count : 0U;
}

uint64_t umi_build_incremental_planner_revision(
    const UmiBuildIncrementalPlanner *planner)
{
    return planner != NULL ? planner->revision : 0U;
}

static void append_reason(char *text, size_t capacity, const char *value)
{
    size_t used;
    if (text == NULL || capacity == 0U || value == NULL) return;
    used = strlen(text);
    if (used > 0U && used + 2U < capacity) {
        text[used++] = ',';
        text[used++] = ' ';
        text[used] = '\0';
    }
    if (used < capacity - 1U) {
        (void)snprintf(text + used, capacity - used, "%s", value);
    }
}

const char *umi_build_incremental_reason_text(
    UmiBuildIncrementalReasonFlags reason_flags,
    char *out_text,
    size_t capacity)
{
    if (out_text == NULL || capacity == 0U) return NULL;
    out_text[0] = '\0';
    if (reason_flags == 0U) append_reason(out_text, capacity, "unchanged");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_EXPLICIT) != 0U)
        append_reason(out_text, capacity, "explicit");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_INPUT_CHANGED) != 0U)
        append_reason(out_text, capacity, "input changed");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_DEPENDENCY_CHANGED) != 0U)
        append_reason(out_text, capacity, "dependency changed");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_OUTPUT_MISSING) != 0U)
        append_reason(out_text, capacity, "output missing");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_ALWAYS_RUN) != 0U)
        append_reason(out_text, capacity, "always run");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_NON_INCREMENTAL) != 0U)
        append_reason(out_text, capacity, "non-incremental");
    if ((reason_flags & UMI_BUILD_INCREMENTAL_REASON_FORCED) != 0U)
        append_reason(out_text, capacity, "forced");
    return out_text;
}
