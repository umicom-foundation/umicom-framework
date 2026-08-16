/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol_hierarchy.c
 *
 * PURPOSE:
 *   Implement one owned, provider-neutral hierarchy tree and bounded graph
 *   traversal engine shared by call, type and future source insight features.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static int valid_node_kind(UmiEditorSymbolHierarchyNodeKind kind)
{
    return kind >= UMI_EDITOR_SYMBOL_HIERARCHY_ROOT &&
           kind <= UMI_EDITOR_SYMBOL_HIERARCHY_OVERRIDE;
}

static UmiStatus validate_node(const UmiEditorSymbolHierarchyNode *node)
{
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
    if ((node->kind == UMI_EDITOR_SYMBOL_HIERARCHY_ROOT) !=
        (node->parent_node_id[0] == '\0')) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_nodes(UmiEditorSymbolHierarchy *hierarchy,
                               size_t required)
{
    size_t capacity;
    UmiEditorSymbolHierarchyNode *replacement;
    if (required <= hierarchy->capacity) return UMI_STATUS_OK;
    capacity = hierarchy->capacity > 0U ? hierarchy->capacity : 64U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSymbolHierarchyNode *)realloc(
        hierarchy->nodes, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    hierarchy->nodes = replacement;
    hierarchy->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_node(const UmiEditorSymbolHierarchy *hierarchy,
                        const char *node_id)
{
    size_t index;
    if (hierarchy == NULL || node_id == NULL) return SIZE_MAX;
    for (index = 0U; index < hierarchy->count; ++index) {
        if (strcmp(hierarchy->nodes[index].node_id, node_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static void refresh_visibility(UmiEditorSymbolHierarchy *hierarchy)
{
    size_t index;
    size_t first_visible = SIZE_MAX;
    for (index = 0U; index < hierarchy->count; ++index) {
        UmiEditorSymbolHierarchyNode *node = &hierarchy->nodes[index];
        if (node->parent_node_id[0] == '\0') {
            node->visible = 1;
        } else {
            size_t parent_index = find_node(hierarchy,
                                            node->parent_node_id);
            node->visible = parent_index < index &&
                hierarchy->nodes[parent_index].visible &&
                hierarchy->nodes[parent_index].expanded;
        }
        if (node->visible && first_visible == SIZE_MAX) first_visible = index;
    }
    if (hierarchy->selected_index >= hierarchy->count ||
        !hierarchy->nodes[hierarchy->selected_index].visible) {
        if (hierarchy->selected_index < hierarchy->count) {
            hierarchy->nodes[hierarchy->selected_index].selected = 0;
        }
        hierarchy->selected_index = first_visible;
        if (first_visible < hierarchy->count) {
            hierarchy->nodes[first_visible].selected = 1;
        }
    }
}

UmiStatus umi_editor_symbol_hierarchy_create(
    UmiEditorSymbolHierarchy **out_hierarchy)
{
    UmiEditorSymbolHierarchy *hierarchy;
    if (out_hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_hierarchy = NULL;
    hierarchy = (UmiEditorSymbolHierarchy *)calloc(1U, sizeof(*hierarchy));
    if (hierarchy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    hierarchy->selected_index = SIZE_MAX;
    hierarchy->revision = 1U;
    *out_hierarchy = hierarchy;
    return UMI_STATUS_OK;
}

void umi_editor_symbol_hierarchy_destroy(UmiEditorSymbolHierarchy *hierarchy)
{
    if (hierarchy == NULL) return;
    free(hierarchy->nodes);
    hierarchy->nodes = NULL;
    free(hierarchy);
}

UmiStatus umi_editor_symbol_hierarchy_clear(
    UmiEditorSymbolHierarchy *hierarchy)
{
    if (hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    hierarchy->count = 0U;
    hierarchy->selected_index = SIZE_MAX;
    hierarchy->truncated = 0;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_add(
    UmiEditorSymbolHierarchy *hierarchy,
    const UmiEditorSymbolHierarchyNode *node)
{
    UmiStatus status;
    if (hierarchy == NULL || validate_node(node) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_node(hierarchy, node->node_id) != SIZE_MAX) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    status = reserve_nodes(hierarchy, hierarchy->count + 1U);
    if (status != UMI_STATUS_OK) return status;
    hierarchy->nodes[hierarchy->count++] = *node;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_finalize(
    UmiEditorSymbolHierarchy *hierarchy,
    int truncated)
{
    size_t index;
    size_t selected = SIZE_MAX;
    if (hierarchy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < hierarchy->count; ++index) {
        hierarchy->nodes[index].child_count = 0U;
        if (hierarchy->nodes[index].selected && selected == SIZE_MAX) {
            selected = index;
        } else {
            hierarchy->nodes[index].selected = 0;
        }
    }
    for (index = 0U; index < hierarchy->count; ++index) {
        UmiEditorSymbolHierarchyNode *node = &hierarchy->nodes[index];
        if (node->parent_node_id[0] != '\0') {
            size_t parent_index = find_node(hierarchy,
                                            node->parent_node_id);
            if (parent_index >= index) return UMI_STATUS_INVALID_STATE;
            hierarchy->nodes[parent_index].child_count += 1U;
        }
    }
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

static UmiStatus validate_rule(const UmiEditorSymbolHierarchyRule *rule)
{
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

UmiStatus umi_editor_symbol_hierarchy_build_options_initialize(
    UmiEditorSymbolHierarchyBuildOptions *options)
{
    if (options == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    options->struct_size = (uint32_t)sizeof(*options);
    options->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
    options->maximum_depth = UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_DEPTH;
    options->maximum_nodes = UMI_EDITOR_HIERARCHY_DEFAULT_MAXIMUM_NODES;
    options->include_indirect = 1;
    return UMI_STATUS_OK;
}

static UmiStatus normalize_options(
    const UmiEditorSymbolHierarchyBuildOptions *provided,
    UmiEditorSymbolHierarchyBuildOptions *normalized)
{
    UmiStatus status;
    status = umi_editor_symbol_hierarchy_build_options_initialize(normalized);
    if (status != UMI_STATUS_OK || provided == NULL) return status;
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

static size_t find_candidate(const HierarchyCandidate *candidates,
                             size_t count,
                             const char *symbol_id,
                             UmiEditorSymbolHierarchyNodeKind kind)
{
    size_t index;
    for (index = 0U; index < count; ++index) {
        if (candidates[index].kind == kind &&
            strcmp(candidates[index].symbol_id, symbol_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static int compare_candidate(const void *left, const void *right)
{
    const HierarchyCandidate *left_candidate =
        (const HierarchyCandidate *)left;
    const HierarchyCandidate *right_candidate =
        (const HierarchyCandidate *)right;
    int location_order = umi_editor_source_location_compare(
        &left_candidate->location, &right_candidate->location);
    if (location_order != 0) return location_order;
    return strcmp(left_candidate->symbol_id, right_candidate->symbol_id);
}

static int is_ancestor(const HierarchyBuildContext *context,
                       const char *symbol_id,
                       size_t ancestor_count)
{
    size_t index;
    for (index = 0U; index < ancestor_count; ++index) {
        if (strcmp(context->ancestors[index], symbol_id) == 0) return 1;
    }
    return 0;
}

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
    if (relationship->kind != rule->relationship_kind) return UMI_STATUS_OK;
    match_id = rule->incoming ? relationship->target_symbol_id
                              : relationship->source_symbol_id;
    child_id = rule->incoming ? relationship->source_symbol_id
                              : relationship->target_symbol_id;
    if (strcmp(match_id, current_symbol_id) != 0) return UMI_STATUS_OK;
    (void)memset(out_candidate, 0, sizeof(*out_candidate));
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
    if (edge_count > 0U) {
        if (edge_count > SIZE_MAX / sizeof(*candidates)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        candidates = (HierarchyCandidate *)calloc(edge_count,
                                                   sizeof(*candidates));
        if (candidates == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (edge_index = 0U; edge_index < edge_count; ++edge_index) {
        UmiEditorSymbolRelationship relationship;
        size_t rule_index;
        UmiStatus status = umi_editor_symbol_relationship_graph_at(
            context->relationship_graph, edge_index, &relationship);
        if (status != UMI_STATUS_OK) {
            free(candidates);
            return status;
        }
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
            if (status != UMI_STATUS_OK) {
                free(candidates);
                return status;
            }
            if (!matches) continue;
            existing = find_candidate(candidates,
                                      candidate_count,
                                      candidate.symbol_id,
                                      candidate.kind);
            if (existing < candidate_count) {
                candidates[existing].relationship_count += 1U;
            } else {
                candidates[candidate_count++] = candidate;
            }
        }
    }
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
    if (symbol_status == UMI_STATUS_OK) {
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
        if (out_node->provider_id[0] == '\0') {
            if (copy_text(out_node->provider_id,
                          sizeof(out_node->provider_id),
                          symbol.provider_id) != UMI_STATUS_OK) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
    } else {
        if (copy_text(out_node->label,
                      sizeof(out_node->label),
                      candidate->symbol_id) != UMI_STATUS_OK) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        out_node->symbol_kind = UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN;
    }
    return UMI_STATUS_OK;
}

static UmiStatus build_children(HierarchyBuildContext *context,
                                const char *current_symbol_id,
                                const char *parent_node_id,
                                size_t depth)
{
    HierarchyCandidate *candidates = NULL;
    size_t candidate_count = 0U;
    size_t index;
    UmiStatus status;
    if (depth >= context->options.maximum_depth) return UMI_STATUS_OK;
    status = collect_candidates(context,
                                current_symbol_id,
                                &candidates,
                                &candidate_count);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < candidate_count; ++index) {
        UmiEditorSymbolHierarchyNode node;
        int cycle;
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
        if (status != UMI_STATUS_OK) break;
        status = umi_editor_symbol_hierarchy_add(context->hierarchy, &node);
        if (status != UMI_STATUS_OK) break;
        if (context->options.include_indirect && !cycle &&
            depth + 1U < context->options.maximum_depth) {
            status = copy_text(context->ancestors[depth + 1U],
                               sizeof(context->ancestors[depth + 1U]),
                               candidates[index].symbol_id);
            if (status != UMI_STATUS_OK) break;
            status = build_children(context,
                                    candidates[index].symbol_id,
                                    node.node_id,
                                    depth + 1U);
            context->ancestors[depth + 1U][0] = '\0';
            if (status != UMI_STATUS_OK) break;
        }
    }
    free(candidates);
    return status;
}

static UmiStatus initialize_root_node(
    const UmiEditorIndexedSymbol *symbol,
    UmiEditorSymbolHierarchyNode *out_node)
{
    (void)memset(out_node, 0, sizeof(*out_node));
    out_node->struct_size = (uint32_t)sizeof(*out_node);
    out_node->api_version = UMI_EDITOR_SYMBOL_HIERARCHY_API_VERSION;
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
    if (hierarchy == NULL || symbol_index == NULL ||
        relationship_graph == NULL || root_symbol_id == NULL ||
        root_symbol_id[0] == '\0' || rules == NULL || rule_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (rule_index = 0U; rule_index < rule_count; ++rule_index) {
        if (validate_rule(&rules[rule_index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    (void)memset(&context, 0, sizeof(context));
    status = normalize_options(options, &context.options);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_index_find(symbol_index,
                                          root_symbol_id,
                                          &root_symbol);
    if (status != UMI_STATUS_OK) return status;
    status = initialize_root_node(&root_symbol, &root_node);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_hierarchy_clear(hierarchy);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_symbol_hierarchy_add(hierarchy, &root_node);
    if (status != UMI_STATUS_OK) return status;
    context.hierarchy = hierarchy;
    context.symbol_index = symbol_index;
    context.relationship_graph = relationship_graph;
    context.rules = rules;
    context.rule_count = rule_count;
    status = copy_text(context.ancestors[0],
                       sizeof(context.ancestors[0]),
                       root_symbol_id);
    if (status == UMI_STATUS_OK) {
        status = build_children(&context,
                                root_symbol_id,
                                root_node.node_id,
                                0U);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_symbol_hierarchy_finalize(hierarchy,
                                                context.truncated);
}

UmiStatus umi_editor_symbol_hierarchy_select(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id)
{
    size_t index;
    if (hierarchy == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(hierarchy, node_id);
    if (index == SIZE_MAX || !hierarchy->nodes[index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (hierarchy->selected_index < hierarchy->count) {
        hierarchy->nodes[hierarchy->selected_index].selected = 0;
    }
    hierarchy->selected_index = index;
    hierarchy->nodes[index].selected = 1;
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_set_expanded(
    UmiEditorSymbolHierarchy *hierarchy,
    const char *node_id,
    int expanded)
{
    size_t index;
    if (hierarchy == NULL || node_id == NULL || node_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_node(hierarchy, node_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    hierarchy->nodes[index].expanded = expanded != 0;
    refresh_visibility(hierarchy);
    hierarchy->revision = next_revision(hierarchy->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t index,
    UmiEditorSymbolHierarchyNode *out_node)
{
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= hierarchy->count) return UMI_STATUS_NOT_FOUND;
    *out_node = hierarchy->nodes[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_visible_at(
    const UmiEditorSymbolHierarchy *hierarchy,
    size_t visible_index,
    UmiEditorSymbolHierarchyNode *out_node)
{
    size_t index;
    size_t current = 0U;
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < hierarchy->count; ++index) {
        if (!hierarchy->nodes[index].visible) continue;
        if (current == visible_index) {
            *out_node = hierarchy->nodes[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_symbol_hierarchy_selected(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchyNode *out_node)
{
    if (hierarchy == NULL || out_node == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (hierarchy->selected_index >= hierarchy->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_node = hierarchy->nodes[hierarchy->selected_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_hierarchy_snapshot(
    const UmiEditorSymbolHierarchy *hierarchy,
    UmiEditorSymbolHierarchySnapshot *out_snapshot)
{
    size_t cycle_count = 0U;
    size_t index;
    if (hierarchy == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < hierarchy->count; ++index) {
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

size_t umi_editor_symbol_hierarchy_count(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    return hierarchy != NULL ? hierarchy->count : 0U;
}

size_t umi_editor_symbol_hierarchy_visible_count(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    size_t count = 0U;
    size_t index;
    if (hierarchy == NULL) return 0U;
    for (index = 0U; index < hierarchy->count; ++index) {
        if (hierarchy->nodes[index].visible) count += 1U;
    }
    return count;
}

uint64_t umi_editor_symbol_hierarchy_revision(
    const UmiEditorSymbolHierarchy *hierarchy)
{
    return hierarchy != NULL ? hierarchy->revision : 0U;
}
