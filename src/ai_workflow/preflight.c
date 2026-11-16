/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workflow/preflight.c
 *
 * PURPOSE:
 *   Validate typed workflow connections and policies, then reuse canonical Framework dependency planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/preflight.h"
#include <stdlib.h>
#include <string.h>

/* One owned temporary workspace keeps large bounded graph storage off the
 * caller stack. No partial candidate becomes visible before the final copy. */
typedef struct UmiAiWorkflowWorkspace {
    UmiFabricWorkflowGraph graph;
    UmiAiWorkflowPlan candidate;
    uint64_t bound_inputs[UMI_AI_WORKFLOW_MAX_NODES];
    bool selected[UMI_AI_WORKFLOW_MAX_NODES];
} UmiAiWorkflowWorkspace;

static UmiStatus workflow_refuse(UmiAiWorkflowDiagnostic *diagnostic,
    UmiAiWorkflowIssue issue, UmiStatus status, size_t node, size_t link,
    size_t port, size_t type)
{
    *diagnostic = (UmiAiWorkflowDiagnostic){issue, node, link, port, type};
    return status;
}

static bool workflow_identifier(const char *text)
{
    const char *end = memchr(text, '\0', UMI_AI_WORKFLOW_ID_CAPACITY);
    if (end == NULL || end == text) return false;
    for (const char *p = text; p != end; ++p) {
        unsigned char c = (unsigned char)*p;
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '-'))
            return false;
    }
    return true;
}

static bool workflow_kind(UmiAiWorkflowValueKind kind)
{
    return kind >= UMI_AI_WORKFLOW_TEXT && kind <= UMI_AI_WORKFLOW_BOOLEAN;
}

static UmiStatus workflow_ports(const UmiAiWorkflowPort *ports, size_t count,
    size_t type_index, UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t p = 0U; p < count; ++p) {
        if (!workflow_identifier(ports[p].name))
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_IDENTIFIER,
                UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, p, type_index);
        if (!workflow_kind(ports[p].kind))
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_VALUE_KIND,
                UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, p, type_index);
        for (size_t previous = 0U; previous < p; ++previous)
            if (strcmp(ports[previous].name, ports[p].name) == 0)
                return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_DUPLICATE,
                    UMI_STATUS_ALREADY_EXISTS, SIZE_MAX, SIZE_MAX, p, type_index);
    }
    return UMI_STATUS_OK;
}

static UmiStatus workflow_types(const UmiAiWorkflowDescription *description,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t t = 0U; t < description->type_count; ++t) {
        const UmiAiWorkflowNodeType *type = &description->types[t];
        if (type->input_count > UMI_AI_WORKFLOW_MAX_PORTS ||
            type->output_count > UMI_AI_WORKFLOW_MAX_PORTS)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_CAPACITY,
                UMI_STATUS_CAPACITY_EXCEEDED, SIZE_MAX, SIZE_MAX, SIZE_MAX, t);
        if (!workflow_identifier(type->operation_id))
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_IDENTIFIER,
                UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, SIZE_MAX, t);
        if ((type->effects & ~UMI_AI_WORKFLOW_KNOWN_EFFECTS) != 0U)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_EFFECT_DEFINITION,
                UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, SIZE_MAX, t);
        for (size_t previous = 0U; previous < t; ++previous)
            if (strcmp(description->types[previous].operation_id, type->operation_id) == 0)
                return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_DUPLICATE,
                    UMI_STATUS_ALREADY_EXISTS, SIZE_MAX, SIZE_MAX, SIZE_MAX, t);
        UmiStatus status = workflow_ports(type->inputs, type->input_count, t, diagnostic);
        if (status != UMI_STATUS_OK) return status;
        status = workflow_ports(type->outputs, type->output_count, t, diagnostic);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static UmiStatus workflow_nodes(const UmiAiWorkflowDescription *description,
    UmiAiWorkflowWorkspace *workspace, UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t n = 0U; n < description->node_count; ++n) {
        const UmiAiWorkflowNode *node = &description->nodes[n];
        if (!workflow_identifier(node->node_id))
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_IDENTIFIER,
                UMI_STATUS_INVALID_ARGUMENT, n, SIZE_MAX, SIZE_MAX, SIZE_MAX);
        if (node->type_index >= description->type_count)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_NODE_TYPE,
                UMI_STATUS_INVALID_ARGUMENT, n, SIZE_MAX, SIZE_MAX, node->type_index);
        for (size_t previous = 0U; previous < n; ++previous)
            if (strcmp(description->nodes[previous].node_id, node->node_id) == 0)
                return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_DUPLICATE,
                    UMI_STATUS_ALREADY_EXISTS, n, SIZE_MAX, SIZE_MAX, node->type_index);
        const UmiAiWorkflowNodeType *type = &description->types[node->type_index];
        /* The type's count has already been bounded to eight, so this shift
         * never approaches the width of the mask. */
        uint64_t mask = (UINT64_C(1) << type->input_count) - UINT64_C(1);
        if ((node->supplied_inputs & ~mask) != 0U)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_BINDING,
                UMI_STATUS_INVALID_ARGUMENT, n, SIZE_MAX, SIZE_MAX, node->type_index);
        workspace->bound_inputs[n] = node->supplied_inputs;
    }
    return UMI_STATUS_OK;
}

static UmiStatus workflow_links(const UmiAiWorkflowDescription *description,
    UmiAiWorkflowWorkspace *workspace, UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t e = 0U; e < description->link_count; ++e) {
        const UmiAiWorkflowLink *link = &description->links[e];
        if (link->source_node >= description->node_count ||
            link->target_node >= description->node_count)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_LINK_NODE,
                UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, e, SIZE_MAX, SIZE_MAX);
        size_t source_type = description->nodes[link->source_node].type_index;
        size_t target_type = description->nodes[link->target_node].type_index;
        const UmiAiWorkflowNodeType *source = &description->types[source_type];
        const UmiAiWorkflowNodeType *target = &description->types[target_type];
        if (link->source_port >= source->output_count)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_PORT,
                UMI_STATUS_INVALID_ARGUMENT, link->source_node, e, link->source_port, source_type);
        if (link->target_port >= target->input_count)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_PORT,
                UMI_STATUS_INVALID_ARGUMENT, link->target_node, e, link->target_port, target_type);
        if (source->outputs[link->source_port].kind != target->inputs[link->target_port].kind)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_TYPE_MISMATCH,
                UMI_STATUS_INVALID_ARGUMENT, link->target_node, e, link->target_port, target_type);
        uint64_t bit = UINT64_C(1) << link->target_port;
        if ((workspace->bound_inputs[link->target_node] & bit) != 0U)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_BINDING,
                UMI_STATUS_INVALID_ARGUMENT, link->target_node, e, link->target_port, target_type);
        workspace->bound_inputs[link->target_node] |= bit;
    }
    return UMI_STATUS_OK;
}

static UmiStatus workflow_select(const UmiAiWorkflowDescription *description,
    UmiAiWorkflowWorkspace *workspace, UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t o = 0U; o < description->output_count; ++o) {
        size_t node = description->output_nodes[o];
        if (node >= description->node_count || workspace->selected[node])
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_OUTPUT,
                UMI_STATUS_INVALID_ARGUMENT, node, SIZE_MAX, SIZE_MAX, SIZE_MAX);
        workspace->selected[node] = true;
    }
    /* Closure marks dependencies, not an execution order. The canonical graph
     * planner remains the single owner of ordering and cycle rules. */
    bool changed;
    do {
        changed = false;
        for (size_t e = 0U; e < description->link_count; ++e) {
            const UmiAiWorkflowLink *link = &description->links[e];
            if (workspace->selected[link->target_node] && !workspace->selected[link->source_node]) {
                workspace->selected[link->source_node] = true;
                changed = true;
            }
        }
    } while (changed);
    return UMI_STATUS_OK;
}

static void workflow_graph(const UmiAiWorkflowDescription *description,
    UmiAiWorkflowWorkspace *workspace, bool selected_only)
{
    UmiFabricWorkflowGraph *graph = &workspace->graph;
    umi_fabric_workflow_graph_init(graph);
    /* The adapter owns this temporary value and has validated every field.
     * Direct bounded construction avoids allocating a constructor per node.
     * The Framework graph validator checks the assembled value before use. */
    for (size_t n = 0U; n < description->node_count; ++n) {
        if (selected_only && !workspace->selected[n]) continue;
        UmiFabricWorkflowStep *step = &graph->steps[graph->step_count++];
        const UmiAiWorkflowNode *node = &description->nodes[n];
        const UmiAiWorkflowNodeType *type = &description->types[node->type_index];
        memcpy(step->step_id, node->node_id, strlen(node->node_id) + 1U);
        memcpy(step->operation_id, type->operation_id, strlen(type->operation_id) + 1U);
        /* No execution timeout is promised by preflight. One satisfies the
         * shared graph's positive metadata contract for dependency planning. */
        step->timeout_ms = 1U;
    }
    for (size_t e = 0U; e < description->link_count; ++e) {
        const UmiAiWorkflowLink *link = &description->links[e];
        if (selected_only && !workspace->selected[link->target_node]) continue;
        UmiFabricWorkflowEdge *edge = &graph->edges[graph->edge_count++];
        const char *before = description->nodes[link->source_node].node_id;
        const char *after = description->nodes[link->target_node].node_id;
        memcpy(edge->before_id, before, strlen(before) + 1U);
        memcpy(edge->after_id, after, strlen(after) + 1U);
    }
}

static UmiStatus workflow_selected_policy(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowWorkspace *workspace,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    for (size_t n = 0U; n < description->node_count; ++n) {
        if (!workspace->selected[n]) continue;
        size_t t = description->nodes[n].type_index;
        const UmiAiWorkflowNodeType *type = &description->types[t];
        for (size_t p = 0U; p < type->input_count; ++p)
            if (type->inputs[p].required &&
                (workspace->bound_inputs[n] & (UINT64_C(1) << p)) == 0U)
                return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_REQUIRED_INPUT,
                    UMI_STATUS_INVALID_ARGUMENT, n, SIZE_MAX, p, t);
        if ((type->effects & ~allowed_effects) != 0U)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_EFFECT_DENIED,
                UMI_STATUS_PERMISSION_DENIED, n, SIZE_MAX, SIZE_MAX, t);
        workspace->candidate.required_effects |= type->effects;
    }
    return UMI_STATUS_OK;
}

static UmiStatus workflow_admit(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowWorkspace *workspace,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    UmiStatus status = workflow_types(description, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    status = workflow_nodes(description, workspace, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    status = workflow_links(description, workspace, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    workflow_graph(description, workspace, false);
    bool cycle = false;
    status = umi_fabric_workflow_graph_has_cycle_checked(&workspace->graph, &cycle);
    if (status != UMI_STATUS_OK || cycle)
        return workflow_refuse(diagnostic,
            cycle ? UMI_AI_WORKFLOW_ISSUE_CYCLE : UMI_AI_WORKFLOW_ISSUE_GRAPH,
            cycle ? UMI_STATUS_INVALID_ARGUMENT : status, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    status = workflow_select(description, workspace, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    status = workflow_selected_policy(description, allowed_effects, workspace, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    workflow_graph(description, workspace, true);
    status = umi_fabric_workflow_plan_build(&workspace->graph, &workspace->candidate.order);
    if (status != UMI_STATUS_OK)
        return workflow_refuse(diagnostic,
            status == UMI_STATUS_OUT_OF_MEMORY ? UMI_AI_WORKFLOW_ISSUE_ALLOCATION : UMI_AI_WORKFLOW_ISSUE_GRAPH,
            status, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    for (size_t i = 0U; i < workspace->candidate.order.count; ++i) {
        size_t n;
        for (n = 0U; n < description->node_count; ++n)
            if (strcmp(workspace->candidate.order.step_ids[i], description->nodes[n].node_id) == 0) break;
        if (n == description->node_count)
            return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_GRAPH,
                UMI_STATUS_INTERNAL_ERROR, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
        workspace->candidate.node_indices[i] = n;
        if (!description->types[description->nodes[n].type_index].available)
            workspace->candidate.unavailable_indices[workspace->candidate.unavailable_count++] = n;
    }
    workspace->candidate.execution_ready = workspace->candidate.unavailable_count == 0U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_workflow_preflight(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowPlan *out_plan,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    if (diagnostic == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *diagnostic = (UmiAiWorkflowDiagnostic){UMI_AI_WORKFLOW_ISSUE_NONE,
        SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX};
    if (description == NULL || out_plan == NULL)
        return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_ARGUMENT,
            UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    if (description->type_count > UMI_AI_WORKFLOW_MAX_TYPES ||
        description->node_count > UMI_AI_WORKFLOW_MAX_NODES ||
        description->link_count > UMI_AI_WORKFLOW_MAX_LINKS ||
        description->output_count > UMI_AI_WORKFLOW_MAX_NODES)
        return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_CAPACITY,
            UMI_STATUS_CAPACITY_EXCEEDED, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    if (description->types == NULL || description->nodes == NULL ||
        description->output_nodes == NULL ||
        (description->link_count != 0U && description->links == NULL) ||
        description->type_count == 0U || description->node_count == 0U ||
        description->output_count == 0U)
        return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_ARGUMENT,
            UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    if ((allowed_effects & ~UMI_AI_WORKFLOW_KNOWN_EFFECTS) != 0U)
        return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_EFFECT_DEFINITION,
            UMI_STATUS_INVALID_ARGUMENT, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    UmiAiWorkflowWorkspace *workspace = calloc(1U, sizeof(*workspace));
    if (workspace == NULL)
        return workflow_refuse(diagnostic, UMI_AI_WORKFLOW_ISSUE_ALLOCATION,
            UMI_STATUS_OUT_OF_MEMORY, SIZE_MAX, SIZE_MAX, SIZE_MAX, SIZE_MAX);
    UmiStatus status = workflow_admit(description, allowed_effects, workspace, diagnostic);
    if (status == UMI_STATUS_OK) memcpy(out_plan, &workspace->candidate, sizeof(*out_plan));
    free(workspace);
    return status;
}
