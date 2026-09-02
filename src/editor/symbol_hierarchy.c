/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol_hierarchy.c
 *
 * PURPOSE:
 *   Implement one owned, provider-neutral hierarchy tree and bounded graph
 *   traversal engine shared by call, type and future source insight features.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/symbol_hierarchy.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HierarchyCandidate {
    char symbol_id[UMI_EDITOR_SYMBOL_ID_CAPACITY];
    char provider_id[UMI_EDITOR_SYMBOL_PROVIDER_CAPACITY];
    UmiEditorSymbolHierarchyNodeKind kind;
    UmiEditorSourceLocation location;
    size_t relationship_count;
} HierarchyCandidate;

typedef struct HierarchyBuildContext {
    UmiEditorSymbolHierarchy *hierarchy;
    const UmiEditorSymbolIndex *symbol_index;
    const UmiEditorSymbolRelationshipGraph *relationship_graph;
    const UmiEditorSymbolHierarchyRule *rules;
    size_t rule_count;
    UmiEditorSymbolHierarchyBuildOptions options;
    char ancestors[UMI_EDITOR_HIERARCHY_MAXIMUM_DEPTH + 1U]
                  [UMI_EDITOR_SYMBOL_ID_CAPACITY];
    int truncated;
} HierarchyBuildContext;

struct UmiEditorSymbolHierarchy {
    UmiEditorSymbolHierarchyNode *nodes;
    size_t count;
    size_t capacity;
    size_t selected_index;
    uint64_t revision;
    int truncated;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the valid node kind operation used by this module and its client applications. */
static int valid_node_kind(UmiEditorSymbolHierarchyNodeKind kind)
{
    return kind >= UMI_EDITOR_SYMBOL_HIERARCHY_ROOT &&
           kind <= UMI_EDITOR_SYMBOL_HIERARCHY_OVERRIDE;
}

/* Provide the validate node operation used by this module and its client applications. */
static UmiStatus validate_node(const UmiEditorSymbolHierarchyNode *node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (node == NULL ||
        node->struct_size != (uint32_t)sizeof(*node) ||
        node->api_version != UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION ||
        !terminated(node->node_id, sizeof(node->node_id)) ||
        node->node_id[0] == '\0' ||
        !terminated(node->parent_node_id, sizeof(node->parent_node_id)) ||
        !terminated(node->symbol_id, sizeof(node->symbol_id)) ||
        node->symbol_id[0] == '\0' ||
        !terminated(node->provider_id, sizeof(node->provider_id)) ||
        !terminated(node->label, sizeof(node->label)) ||
        node->label[0] == '\0' ||
        !terminated(node->detail, sizeof(node->detail)) ||
        !valid_node_kind(node->kind) ||
        node->depth > UMI_EDITOR_HIERARCHY_MAXIMUM_DEPTH ||
        umi_editor_source_location_validate(&node->location) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((node->kind == UMI_EDITOR_SYMBOL_HIERARCHY_ROOT) !=
        (node->parent_node_id[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the reserve nodes operation used by this module and its client applications. */
static UmiStatus reserve_nodes(UmiEditorSymbolHierarchy *hierarchy,
                               size_t required)
{
    size_t capacity;
    UmiEditorSymbolHierarchyNode *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= hierarchy->capacity) return UMI_STATUS_OK;
    capacity = hierarchy->capacity > 0U ? hierarchy->capacity : 64U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSymbolHierarchyNode *)realloc(
        hierarchy->nodes, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    hierarchy->nodes = replacement;
    hierarchy->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find node operation used by this module and its client applications. */
static size_t find_node(const UmiEditorSymbolHierarchy *hierarchy,
                        const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || node_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(hierarchy->nodes[index].node_id, node_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the refresh visibility operation used by this module and its client
 * applications.
 */
static void refresh_visibility(UmiEditorSymbolHierarchy *hierarchy)
{
    size_t index;
    size_t first_visible = SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        UmiEditorSymbolHierarchyNode *node = &hierarchy->nodes[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node->parent_node_id[0] == '\0') {
            node->visible = 1;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            size_t parent_index = find_node(hierarchy,
                                            node->parent_node_id);
            node->visible = parent_index < index &&
                hierarchy->nodes[parent_index].visible &&
                hierarchy->nodes[parent_index].expanded;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (node->visible && first_visible == SIZE_MAX) first_visible = index;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (hierarchy->selected_index >= hierarchy->count ||
        !hierarchy->nodes[hierarchy->selected_index].visible) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hierarchy->selected_index < hierarchy->count) {
            hierarchy->nodes[hierarchy->selected_index].selected = 0;
        }
        hierarchy->selected_index = first_visible;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (first_visible < hierarchy->count) {
            hierarchy->nodes[first_visible].selected = 1;
        }
    }
}

/*
 * Initialise editor symbol hierarchy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_symbol_hierarchy_create(
    UmiEditorSymbolHierarchy **out_hierarchy)
{
    UmiEditorSymbolHierarchy *hierarchy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_hierarchy = NULL;
    hierarchy = (UmiEditorSymbolHierarchy *)calloc(1U, sizeof(*hierarchy));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    hierarchy->selected_index = SIZE_MAX;
    hierarchy->revision = 1U;
    *out_hierarchy = hierarchy;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor symbol hierarchy so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_hierarchy_destroy(UmiEditorSymbolHierarchy *hierarchy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return;
    free(hierarchy->nodes);
    hierarchy->nodes = NULL;
    free(hierarchy);
}

/*
 * Release or reset state held by editor symbol hierarchy so the same storage can be reused
 * safely.
 */
UmiStatus umi_editor_symbol_hierarchy_clear(
    UmiEditorSymbolHierarchy *hierarchy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    hierarchy->count = 0U;
    hierarchy->selected_index = SIZE_MAX;
    hierarchy->truncated = 0;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

/*
 * Add editor symbol hierarchy only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_symbol_hierarchy_add(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolHierarchyNode *node)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || validate_node(node) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_node(hierarchy, node->node_id) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    status = reserve_nodes(hierarchy, hierarchy->count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    hierarchy->nodes[hierarchy->count++] = *node;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol hierarchy finalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_finalize(
    UmiEditorSymbolHierarchy *hierarchy,
    int truncated)
{
    size_t index;
    size_t selected = SIZE_MAX;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        hierarchy->nodes[index].child_count = 0U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hierarchy->nodes[index].selected && selected == SIZE_MAX) {
            selected = index;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            hierarchy->nodes[index].selected = 0;
        }
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        UmiEditorSymbolHierarchyNode *node = &hierarchy->nodes[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (node->parent_node_id[0] != '\0') {
            size_t parent_index = find_node(hierarchy,
                                            node->parent_node_id);
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (parent_index >= index) return UMI_STATUS_INVALID_STATE;
            hierarchy->nodes[parent_index].child_count += 1U;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selected == SIZE_MAX && hierarchy->count > 0U) {
        selected = 0U;
        hierarchy->nodes[0].selected = 1;
    }
    hierarchy->selected_index = selected;
    hierarchy->truncated = truncated != 0;
    refresh_visibility(hierarchy);
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

/* Provide the validate rule operation used by this module and its client applications. */
static UmiStatus validate_rule(const UmiEditorSymbolHierarchyRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL ||
        rule->struct_size != (uint32_t)sizeof(*rule) ||
        rule->api_version != UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION ||
        rule->relationship_kind < UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION ||
        rule->relationship_kind > UMI_EDITOR_SYMBOL_RELATIONSHIP_OVERRIDE ||
        !valid_node_kind(rule->node_kind) ||
        rule->node_kind == UMI_EDITOR_SYMBOL_HIERARCHY_ROOT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise editor symbol hierarchy build options from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_symbol_hierarchy_build_options_initialize(
    UmiEditorSymbolHierarchyBuildOptions *options)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    options->struct_size = (uint32_t)sizeof(*options);
    options->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    options->maximum_depth = UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_DEPTH;
    options->maximum_nodes = UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_NODES;
    options->include_indirect = 1;
    return UMI_STATUS_OK;
}

/* Provide the normalize options operation used by this module and its client applications. */
static UmiStatus normalize_options(
    const UmiEditorSymbolHierarchyBuildOptions *provided,
    UmiEditorSymbolHierarchyBuildOptions *normalized)
{
    UmiStatus status;
    status = umi_editor_symbol_hierarchy_build_options_initialize(normalized);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK || provided == NULL) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (provided->struct_size != (uint32_t)sizeof(*provided) ||
        provided->api_version != UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION ||
        provided->maximum_depth == 0U ||
        provided->maximum_depth > UMI_EDITOR_HIERARCHY_MAXIMUM_DEPTH ||
        provided->maximum_nodes == 0U ||
        provided->maximum_nodes > UMI_EDITOR_HIERARCHY_MAXIMUM_NODES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *normalized = *provided;
    normalized->include_indirect = provided->include_indirect != 0;
    return UMI_STATUS_OK;
}

/* Provide the find candidate operation used by this module and its client applications. */
static size_t find_candidate(const HierarchyCandidate *candidates,
                             size_t count,
                             const char *symbol_id,
                             UmiEditorSymbolHierarchyNodeKind kind)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (candidates[index].kind == kind &&
            strcmp(candidates[index].symbol_id, symbol_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the compare candidate operation used by this module and its client applications. */
static int compare_candidate(const void *left, const void *right)
{
    const HierarchyCandidate *left_candidate =
        (const HierarchyCandidate *)left;
    const HierarchyCandidate *right_candidate =
        (const HierarchyCandidate *)right;
    int location_order = umi_editor_source_location_compare(
        &left_candidate->location, &right_candidate->location);
    /* Apply this branch only when its contract condition is satisfied. */
    if (location_order != 0) return location_order;
    return strcmp(left_candidate->symbol_id, right_candidate->symbol_id);
}

/* Provide the is ancestor operation used by this module and its client applications. */
static int is_ancestor(const HierarchyBuildContext *context,
                       const char *symbol_id,
                       size_t ancestor_count)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < ancestor_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(context->ancestors[index], symbol_id) == 0) return 1;
    }
    return 0;
}

/*
 * Provide the candidate from relationship operation used by this module and its client
 * applications.
 */
static UmiStatus candidate_from_relationship(
    const UmiEditorSymbolRelationship *relationship,
    const UmiEditorSymbolHierarchyRule *rule,
    const char *current_symbol_id,
    HierarchyCandidate *out_candidate,
    int *out_matches)
{
    const char *match_id;
    const char *child_id;
    *out_matches = 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (relationship->kind != rule->relationship_kind) return UMI_STATUS_OK;
    match_id = rule->incoming ? relationship->target_symbol_id
                              : relationship->source_symbol_id;
    child_id = rule->incoming ? relationship->source_symbol_id
                              : relationship->target_symbol_id;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(match_id, current_symbol_id) != 0) return UMI_STATUS_OK;
    (void)memset(out_candidate, 0, sizeof(*out_candidate));
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_text(out_candidate->symbol_id,
                  sizeof(out_candidate->symbol_id),
                  child_id) != UMI_STATUS_OK ||
        copy_text(out_candidate->provider_id,
                  sizeof(out_candidate->provider_id),
                  relationship->provider_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_candidate->kind = rule->node_kind;
    out_candidate->location = rule->incoming
        ? relationship->source_location
        : relationship->target_location;
    out_candidate->relationship_count = 1U;
    *out_matches = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the collect candidates operation used by this module and its client
 * applications.
 */
static UmiStatus collect_candidates(const HierarchyBuildContext *context,
                                    const char *current_symbol_id,
                                    HierarchyCandidate **out_candidates,
                                    size_t *out_count)
{
    size_t edge_count = umi_editor_symbol_relationship_graph_count(
        context->relationship_graph);
    HierarchyCandidate *candidates = NULL;
    size_t candidate_count = 0U;
    size_t edge_index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (edge_count > 0U) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (edge_count > SIZE_MAX / sizeof(*candidates)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        candidates = (HierarchyCandidate *)calloc(edge_count,
                                                   sizeof(*candidates));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (candidates == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (edge_index = 0U; edge_index < edge_count; ++edge_index) {
        UmiEditorSymbolRelationship relationship;
        size_t rule_index;
        UmiStatus status = umi_editor_symbol_relationship_graph_at(
            context->relationship_graph, edge_index, &relationship);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(candidates);
            return status;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (rule_index = 0U; rule_index < context->rule_count; ++rule_index) {
            HierarchyCandidate candidate;
            size_t existing;
            int matches;
            status = candidate_from_relationship(
                &relationship,
                &context->rules[rule_index],
                current_symbol_id,
                &candidate,
                &matches);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                free(candidates);
                return status;
            }
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (!matches) continue;
            existing = find_candidate(candidates,
                                      candidate_count,
                                      candidate.symbol_id,
                                      candidate.kind);
            /* Apply this branch only when its contract condition is satisfied. */
            if (existing < candidate_count) {
                candidates[existing].relationship_count += 1U;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                candidates[candidate_count++] = candidate;
            }
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate_count > 1U) {
        qsort(candidates,
              candidate_count,
              sizeof(*candidates),
              compare_candidate);
    }
    *out_candidates = candidates;
    *out_count = candidate_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the initialize node from candidate operation used by this module and its client
 * applications.
 */
static UmiStatus initialize_node_from_candidate(
    const HierarchyBuildContext *context,
    const HierarchyCandidate *candidate,
    const char *parent_node_id,
    size_t depth,
    int cycle,
    UmiEditorSymbolHierarchyNode *out_node)
{
    UmiEditorIndexedSymbol symbol;
    UmiStatus symbol_status;
    int written;
    (void)memset(out_node, 0, sizeof(*out_node));
    out_node->struct_size = (uint32_t)sizeof(*out_node);
    out_node->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    written = snprintf(out_node->node_id,
                       sizeof(out_node->node_id),
                       "hierarchy:%zu",
                       umi_editor_symbol_hierarchy_count(context->hierarchy));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_node->node_id) ||
        copy_text(out_node->parent_node_id,
                  sizeof(out_node->parent_node_id),
                  parent_node_id) != UMI_STATUS_OK ||
        copy_text(out_node->symbol_id,
                  sizeof(out_node->symbol_id),
                  candidate->symbol_id) != UMI_STATUS_OK ||
        copy_text(out_node->provider_id,
                  sizeof(out_node->provider_id),
                  candidate->provider_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_node->kind = candidate->kind;
    out_node->location = candidate->location;
    out_node->depth = depth;
    out_node->relationship_count = candidate->relationship_count;
    out_node->expanded = 1;
    out_node->cycle = cycle;
    symbol_status = umi_editor_symbol_index_find(context->symbol_index,
                                                  candidate->symbol_id,
                                                  &symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (symbol_status == UMI_STATUS_OK) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (copy_text(out_node->label,
                      sizeof(out_node->label),
                      symbol.name) != UMI_STATUS_OK ||
            copy_text(out_node->detail,
                      sizeof(out_node->detail),
                      symbol.detail) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_node->symbol_kind = symbol.kind;
        out_node->location = symbol.selection_location;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (out_node->provider_id[0] == '\0') {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (copy_text(out_node->provider_id,
                          sizeof(out_node->provider_id),
                          symbol.provider_id) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Apply this branch only when its contract condition is satisfied. */
        if (copy_text(out_node->label,
                      sizeof(out_node->label),
                      candidate->symbol_id) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_node->symbol_kind = UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN;
    }
    return UMI_STATUS_OK;
}

/* Provide the build children operation used by this module and its client applications. */
static UmiStatus build_children(HierarchyBuildContext *context,
                                const char *current_symbol_id,
                                const char *parent_node_id,
                                size_t depth)
{
    HierarchyCandidate *candidates = NULL;
    size_t candidate_count = 0U;
    size_t index;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (depth >= context->options.maximum_depth) return UMI_STATUS_OK;
    status = collect_candidates(context,
                                current_symbol_id,
                                &candidates,
                                &candidate_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < candidate_count; ++index) {
        UmiEditorSymbolHierarchyNode node;
        int cycle;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_symbol_hierarchy_count(context->hierarchy) >=
            context->options.maximum_nodes) {
            context->truncated = 1;
            break;
        }
        cycle = is_ancestor(context,
                            candidates[index].symbol_id,
                            depth + 1U);
        status = initialize_node_from_candidate(context,
                                                &candidates[index],
                                                parent_node_id,
                                                depth + 1U,
                                                cycle,
                                                &node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        status = umi_editor_symbol_hierarchy_add(context->hierarchy, &node);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Apply this branch only when its contract condition is satisfied. */
        if (context->options.include_indirect && !cycle &&
            depth + 1U < context->options.maximum_depth) {
            status = copy_text(context->ancestors[depth + 1U],
                               sizeof(context->ancestors[depth + 1U]),
                               candidates[index].symbol_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
            status = build_children(context,
                                    candidates[index].symbol_id,
                                    node.node_id,
                                    depth + 1U);
            context->ancestors[depth + 1U][0] = '\0';
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
    }
    free(candidates);
    return status;
}

/*
 * Provide the initialize root node operation used by this module and its client
 * applications.
 */
static UmiStatus initialize_root_node(
    const UmiEditorIndexedSymbol *symbol,
    UmiEditorSymbolHierarchyNode *out_node)
{
    (void)memset(out_node, 0, sizeof(*out_node));
    out_node->struct_size = (uint32_t)sizeof(*out_node);
    out_node->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (copy_text(out_node->node_id,
                  sizeof(out_node->node_id),
                  "hierarchy:0") != UMI_STATUS_OK ||
        copy_text(out_node->symbol_id,
                  sizeof(out_node->symbol_id),
                  symbol->id) != UMI_STATUS_OK ||
        copy_text(out_node->provider_id,
                  sizeof(out_node->provider_id),
                  symbol->provider_id) != UMI_STATUS_OK ||
        copy_text(out_node->label,
                  sizeof(out_node->label),
                  symbol->name) != UMI_STATUS_OK ||
        copy_text(out_node->detail,
                  sizeof(out_node->detail),
                  symbol->detail) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    out_node->kind = UMI_EDITOR_SYMBOL_HIERARCHY_ROOT;
    out_node->symbol_kind = symbol->kind;
    out_node->location = symbol->selection_location;
    out_node->relationship_count = 1U;
    out_node->expanded = 1;
    out_node->selected = 1;
    out_node->visible = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol hierarchy build relationships operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_build_relationships(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolIndex *symbol_index,
    const UmiEditorSymbolRelationshipGraph *relationship_graph,
    const char *root_symbol_id,
    const UmiEditorSymbolHierarchyRule *rules,
    size_t rule_count,
    const UmiEditorSymbolHierarchyBuildOptions *options)
{
    HierarchyBuildContext context;
    UmiEditorIndexedSymbol root_symbol;
    UmiEditorSymbolHierarchyNode root_node;
    size_t rule_index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || symbol_index == NULL ||
        relationship_graph == NULL || root_symbol_id == NULL ||
        root_symbol_id[0] == '\0' || rules == NULL || rule_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (rule_index = 0U; rule_index < rule_count; ++rule_index) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (validate_rule(&rules[rule_index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    (void)memset(&context, 0, sizeof(context));
    status = normalize_options(options, &context.options);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_index_find(symbol_index,
                                          root_symbol_id,
                                          &root_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = initialize_root_node(&root_symbol, &root_node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_hierarchy_clear(hierarchy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_hierarchy_add(hierarchy, &root_node);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    context.hierarchy = hierarchy;
    context.symbol_index = symbol_index;
    context.relationship_graph = relationship_graph;
    context.rules = rules;
    context.rule_count = rule_count;
    status = copy_text(context.ancestors[0],
                       sizeof(context.ancestors[0]),
                       root_symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = build_children(&context,
                                root_symbol_id,
                                root_node.node_id,
                                0U);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_symbol_hierarchy_finalize(hierarchy,
                                                context.truncated);
}

/*
 * Provide the editor symbol hierarchy select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_hierarchy_select(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(hierarchy, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX || !hierarchy->nodes[index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (hierarchy->selected_index < hierarchy->count) {
        hierarchy->nodes[hierarchy->selected_index].selected = 0;
    }
    hierarchy->selected_index = index;
    hierarchy->nodes[index].selected = 1;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol hierarchy set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_set_expanded(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id,
    int expanded)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(hierarchy, node_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    hierarchy->nodes[index].expanded = expanded != 0;
    refresh_visibility(hierarchy);
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor symbol hierarchy while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_hierarchy_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t index,
    UmiEditorSymbolHierarchyNode *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= hierarchy->count) return UMI_STATUS_NOT_FOUND;
    *out_node = hierarchy->nodes[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor symbol hierarchy visible while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_hierarchy_visible_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t visible_index,
    UmiEditorSymbolHierarchyNode *out_node)
{
    size_t index;
    size_t current = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!hierarchy->nodes[index].visible) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (current == visible_index) {
            *out_node = hierarchy->nodes[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find editor symbol hierarchy while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_symbol_hierarchy_selected(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchyNode *out_node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (hierarchy->selected_index >= hierarchy->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = hierarchy->nodes[hierarchy->selected_index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol hierarchy snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_hierarchy_snapshot(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchySnapshot *out_snapshot)
{
    size_t cycle_count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hierarchy->nodes[index].cycle) cycle_count += 1U;
    }
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    out_snapshot->node_count = hierarchy->count;
    out_snapshot->visible_node_count =
        umi_editor_symbol_hierarchy_visible_count(hierarchy);
    out_snapshot->selected_index =
        hierarchy->selected_index < hierarchy->count
            ? hierarchy->selected_index
            : 0U;
    out_snapshot->cycle_count = cycle_count;
    out_snapshot->revision = hierarchy->revision;
    out_snapshot->has_selection = hierarchy->selected_index < hierarchy->count;
    out_snapshot->truncated = hierarchy->truncated;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor symbol hierarchy without changing
 * their state.
 */
size_t umi_editor_symbol_hierarchy_count(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    return hierarchy != NULL ? hierarchy->count : 0U;
}

/*
 * Return the number of records represented by editor symbol hierarchy visible without
 * changing their state.
 */
size_t umi_editor_symbol_hierarchy_visible_count(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    size_t count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hierarchy == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hierarchy->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hierarchy->nodes[index].visible) count += 1U;
    }
    return count;
}

/*
 * Provide the editor symbol hierarchy revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_symbol_hierarchy_revision(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    return hierarchy != NULL ? hierarchy->revision : 0U;
}
