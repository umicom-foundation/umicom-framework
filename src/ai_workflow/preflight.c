/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workflow/preflight.c
 *
 * PURPOSE:
 *   Add typed workflow admission on top of canonical Fabric dependency planning.
 *
 * EDUCATIONAL OVERVIEW:
 *   Validate before publication. Work on a bounded heap candidate so failures
 *   cannot damage an earlier plan or overflow a small application thread stack.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT for this original Umicom contribution. No ComfyUI source is included.
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workflow/preflight.h"
#include <stdlib.h>
#include <string.h>

/* Keep large Framework values off the stack. All fields begin zeroed and are
 * temporary; the caller sees only a complete successful plan. */
typedef struct WorkflowScratch {
    UmiFabricWorkflowGraph graph;
    UmiFabricWorkflowPlan whole_order;
    UmiAiWorkflowPlan candidate;
    bool selected[UMI_FABRIC_MAX_STEPS];
    uint32_t linked_inputs[UMI_FABRIC_MAX_STEPS];
} WorkflowScratch;

/* Populate a location-based diagnostic without retaining pointers or private
 * prompt strings. The original status remains available to the caller. */
static UmiStatus refuse(UmiAiWorkflowDiagnostic *diagnostic, UmiStatus status,
    UmiAiWorkflowIssue issue, size_t node, size_t link, size_t port)
{
    if (diagnostic != NULL) {
        diagnostic->issue = issue;
        diagnostic->node_index = node;
        diagnostic->link_index = link;
        diagnostic->port_index = port;
    }
    return status;
}

/* Bound the terminator search before any strcmp/copy. Identifiers are stable
 * ASCII keys, not translated labels, paths, executable names or shell text. */
static bool identifier_valid(const char *text, size_t capacity)
{
    if (text == NULL || capacity == 0U || text[0] == '\0' ||
        memchr(text, '\0', capacity) == NULL) return false;
    for (size_t i = 0U; text[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)text[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-')) return false;
    }
    return true;
}

/* Inputs and outputs have separate name spaces. Exact enum types are required:
 * implicit conversions must be explicit future nodes, never hidden coercions. */
static bool ports_valid(const UmiAiWorkflowPort *ports, size_t count)
{
    for (size_t i = 0U; i < count; ++i) {
        if (!identifier_valid(ports[i].name, sizeof(ports[i].name)) ||
            ports[i].kind < UMI_AI_WORKFLOW_TEXT || ports[i].kind > UMI_AI_WORKFLOW_ASSET)
            return false;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(ports[i].name, ports[j].name) == 0) return false;
    }
    return true;
}

/* Reject corrupt sizes and missing storage before indexing any caller array.
 * Empty workflows have no requested result and are not executable plans. */
static UmiStatus validate_shape(const UmiAiWorkflowDescription *d,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    if (d == NULL || d->types == NULL || d->nodes == NULL || d->requested_outputs == NULL ||
        d->type_count == 0U || d->node_count == 0U || d->requested_output_count == 0U ||
        (d->link_count != 0U && d->links == NULL))
        return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_ARGUMENT, none, none, none);
    if (d->type_count > UMI_AI_WORKFLOW_MAX_TYPES || d->node_count > UMI_FABRIC_MAX_STEPS ||
        d->link_count > UMI_FABRIC_MAX_EDGES || d->requested_output_count > d->node_count)
        return refuse(diagnostic, UMI_STATUS_CAPACITY_EXCEEDED, UMI_AI_WORKFLOW_LIMIT, none, none, none);
    return UMI_STATUS_OK;
}

/* Inspect registry definitions before graph instances. Imported workflows may
 * refer to a definition but cannot declare their own installed capabilities. */
static UmiStatus validate_types(const UmiAiWorkflowDescription *d,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    for (size_t i = 0U; i < d->type_count; ++i) {
        const UmiAiWorkflowNodeType *type = &d->types[i];
        if (!identifier_valid(type->operation_id, sizeof(type->operation_id)))
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_IDENTIFIER, none, none, none);
        if (type->input_count > UMI_AI_WORKFLOW_MAX_PORTS || type->output_count > UMI_AI_WORKFLOW_MAX_PORTS ||
            (type->required_effects & ~UMI_AI_WORKFLOW_ALL_EFFECTS) != 0U)
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_TYPE, none, none, none);
        if (!ports_valid(type->inputs, type->input_count) || !ports_valid(type->outputs, type->output_count))
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_TYPE, none, none, none);
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(type->operation_id, d->types[j].operation_id) == 0)
                return refuse(diagnostic, UMI_STATUS_ALREADY_EXISTS, UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER, none, none, none);
    }
    return UMI_STATUS_OK;
}

/* Node IDs must remain unique for the canonical planner. Supplied-input masks
 * are bounded before shifts, preventing an unknown bit from bypassing a port. */
static UmiStatus validate_nodes(const UmiAiWorkflowDescription *d,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    for (size_t i = 0U; i < d->node_count; ++i) {
        const UmiAiWorkflowNode *node = &d->nodes[i];
        if (!identifier_valid(node->node_id, sizeof(node->node_id)))
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_IDENTIFIER, i, none, none);
        if (node->type_index >= d->type_count)
            return refuse(diagnostic, UMI_STATUS_NOT_FOUND, UMI_AI_WORKFLOW_BAD_TYPE, i, none, none);
        uint32_t valid_bits = (UINT32_C(1) << d->types[node->type_index].input_count) - UINT32_C(1);
        if ((node->supplied_inputs & ~valid_bits) != 0U)
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_TYPE, i, none, none);
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(node->node_id, d->nodes[j].node_id) == 0)
                return refuse(diagnostic, UMI_STATUS_ALREADY_EXISTS, UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER, i, none, none);
    }
    for (size_t i = 0U; i < d->requested_output_count; ++i) {
        if (d->requested_outputs[i] >= d->node_count)
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_ARGUMENT, none, none, none);
        for (size_t j = 0U; j < i; ++j)
            if (d->requested_outputs[i] == d->requested_outputs[j])
                return refuse(diagnostic, UMI_STATUS_ALREADY_EXISTS, UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER,
                    d->requested_outputs[i], none, none);
    }
    return UMI_STATUS_OK;
}

/* Each destination port has one writer. A literal/external binding and a link
 * cannot silently compete; fan-out to distinct destinations remains permitted. */
static UmiStatus validate_links(const UmiAiWorkflowDescription *d, WorkflowScratch *scratch,
    UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    for (size_t i = 0U; i < d->link_count; ++i) {
        const UmiAiWorkflowLink *link = &d->links[i];
        if (link->source_node >= d->node_count || link->target_node >= d->node_count)
            return refuse(diagnostic, UMI_STATUS_NOT_FOUND, UMI_AI_WORKFLOW_BAD_LINK, none, i, none);
        if (link->source_node == link->target_node)
            return refuse(diagnostic, UMI_STATUS_INVALID_STATE, UMI_AI_WORKFLOW_CYCLE, link->target_node, i, none);
        const UmiAiWorkflowNode *source = &d->nodes[link->source_node];
        const UmiAiWorkflowNode *target = &d->nodes[link->target_node];
        const UmiAiWorkflowNodeType *source_type = &d->types[source->type_index];
        const UmiAiWorkflowNodeType *target_type = &d->types[target->type_index];
        if (link->source_port >= source_type->output_count || link->target_port >= target_type->input_count)
            return refuse(diagnostic, UMI_STATUS_NOT_FOUND, UMI_AI_WORKFLOW_BAD_LINK, link->target_node, i, link->target_port);
        if (source_type->outputs[link->source_port].kind != target_type->inputs[link->target_port].kind)
            return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_TYPE_MISMATCH,
                link->target_node, i, link->target_port);
        uint32_t bit = UINT32_C(1) << link->target_port;
        if (((scratch->linked_inputs[link->target_node] | target->supplied_inputs) & bit) != 0U)
            return refuse(diagnostic, UMI_STATUS_ALREADY_EXISTS, UMI_AI_WORKFLOW_DUPLICATE_BINDING,
                link->target_node, i, link->target_port);
        scratch->linked_inputs[link->target_node] |= bit;
    }
    return UMI_STATUS_OK;
}

/* Construct the public inline Fabric value from already checked descriptors.
 * We do not implement a second cycle detector or topological sort. Port-level
 * parallel links collapse to one ordering edge without discarding port checks. */
static UmiStatus build_fabric_graph(const UmiAiWorkflowDescription *d,
    const bool *selection, UmiFabricWorkflowGraph *graph)
{
    memset(graph, 0, sizeof(*graph));
    for (size_t i = 0U; i < d->node_count; ++i) {
        if (selection != NULL && !selection[i]) continue;
        const UmiAiWorkflowNode *node = &d->nodes[i];
        UmiFabricWorkflowStep *step = &graph->steps[graph->step_count];
        UmiStatus status = umi_fabric_copy_text(step->step_id, sizeof(step->step_id), node->node_id);
        if (status != UMI_STATUS_OK) return status;
        status = umi_fabric_copy_text(step->operation_id, sizeof(step->operation_id),
            d->types[node->type_index].operation_id);
        if (status != UMI_STATUS_OK) return status;
        step->timeout_ms = 1U; /* Required Fabric metadata; not an execution timeout policy. */
        ++graph->step_count;
    }
    for (size_t i = 0U; i < d->link_count; ++i) {
        const UmiAiWorkflowLink *link = &d->links[i];
        if (selection != NULL && (!selection[link->source_node] || !selection[link->target_node])) continue;
        const char *before = d->nodes[link->source_node].node_id;
        const char *after = d->nodes[link->target_node].node_id;
        bool duplicate = false;
        for (size_t j = 0U; j < graph->edge_count; ++j)
            if (strcmp(graph->edges[j].before_id, before) == 0 && strcmp(graph->edges[j].after_id, after) == 0)
                duplicate = true;
        if (duplicate) continue;
        UmiFabricWorkflowEdge *edge = &graph->edges[graph->edge_count];
        UmiStatus status = umi_fabric_copy_text(edge->before_id, sizeof(edge->before_id), before);
        if (status != UMI_STATUS_OK) return status;
        status = umi_fabric_copy_text(edge->after_id, sizeof(edge->after_id), after);
        if (status != UMI_STATUS_OK) return status;
        ++graph->edge_count;
    }
    return UMI_STATUS_OK;
}

/* A bounded reverse reachability pass selects prerequisites of requested
 * outputs. Cycles have already been refused, but the pass bound also ensures
 * predictable work independently of edge insertion order. */
static void select_required_nodes(const UmiAiWorkflowDescription *d, WorkflowScratch *scratch)
{
    for (size_t i = 0U; i < d->requested_output_count; ++i)
        scratch->selected[d->requested_outputs[i]] = true;
    for (size_t pass = 0U; pass < d->node_count; ++pass) {
        bool changed = false;
        for (size_t i = 0U; i < d->link_count; ++i) {
            const UmiAiWorkflowLink *link = &d->links[i];
            if (scratch->selected[link->target_node] && !scratch->selected[link->source_node]) {
                scratch->selected[link->source_node] = true;
                changed = true;
            }
        }
        if (!changed) break;
    }
}

/* Runtime readiness is narrower than structural validity. An inactive branch
 * needs no loaded provider; an active unsupported operation remains explicit.
 * Consent is checked before even describing an executable active plan. */
static UmiStatus check_active_contracts(const UmiAiWorkflowDescription *d, uint32_t allowed,
    WorkflowScratch *scratch, UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    for (size_t i = 0U; i < d->node_count; ++i) {
        if (!scratch->selected[i]) { ++scratch->candidate.omitted_count; continue; }
        const UmiAiWorkflowNode *node = &d->nodes[i];
        const UmiAiWorkflowNodeType *type = &d->types[node->type_index];
        uint32_t supplied = node->supplied_inputs | scratch->linked_inputs[i];
        for (size_t j = 0U; j < type->input_count; ++j)
            if (type->inputs[j].required && (supplied & (UINT32_C(1) << j)) == 0U)
                return refuse(diagnostic, UMI_STATUS_INVALID_STATE, UMI_AI_WORKFLOW_MISSING_INPUT, i, none, j);
        if ((type->required_effects & ~allowed) != 0U)
            return refuse(diagnostic, UMI_STATUS_PERMISSION_DENIED, UMI_AI_WORKFLOW_EFFECT_DENIED, i, none, none);
        scratch->candidate.required_effects |= type->required_effects;
        if (!type->available)
            scratch->candidate.unavailable_indices[scratch->candidate.unavailable_count++] = i;
    }
    scratch->candidate.execution_ready = scratch->candidate.unavailable_count == 0U;
    return UMI_STATUS_OK;
}

/* Stage, validate, plan, then publish exactly once. No caller state is modified
 * along error paths, and the same canonical planner orders both full and sliced
 * graphs. Callers must recheck live provider/policy state when executing later. */
UmiStatus umi_ai_workflow_preflight(const UmiAiWorkflowDescription *d,
    uint32_t allowed_effects, UmiAiWorkflowPlan *out_plan, UmiAiWorkflowDiagnostic *diagnostic)
{
    const size_t none = UMI_AI_WORKFLOW_NO_INDEX;
    (void)refuse(diagnostic, UMI_STATUS_OK, UMI_AI_WORKFLOW_VALID, none, none, none);
    if (out_plan == NULL || (allowed_effects & ~UMI_AI_WORKFLOW_ALL_EFFECTS) != 0U)
        return refuse(diagnostic, UMI_STATUS_INVALID_ARGUMENT, UMI_AI_WORKFLOW_BAD_ARGUMENT, none, none, none);
    UmiStatus status = validate_shape(d, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    status = validate_types(d, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    status = validate_nodes(d, diagnostic);
    if (status != UMI_STATUS_OK) return status;
    WorkflowScratch *scratch = calloc(1U, sizeof(*scratch));
    if (scratch == NULL)
        return refuse(diagnostic, UMI_STATUS_OUT_OF_MEMORY, UMI_AI_WORKFLOW_DEPENDENCY_ERROR, none, none, none);
    status = validate_links(d, scratch, diagnostic);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = build_fabric_graph(d, NULL, &scratch->graph);
    if (status != UMI_STATUS_OK) goto dependency_error;
    if (umi_fabric_workflow_graph_has_cycle(&scratch->graph)) {
        status = refuse(diagnostic, UMI_STATUS_INVALID_STATE, UMI_AI_WORKFLOW_CYCLE, none, none, none);
        goto cleanup;
    }
    status = umi_fabric_workflow_plan_build(&scratch->graph, &scratch->whole_order);
    if (status != UMI_STATUS_OK) goto dependency_error;
    select_required_nodes(d, scratch);
    status = check_active_contracts(d, allowed_effects, scratch, diagnostic);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = build_fabric_graph(d, scratch->selected, &scratch->graph);
    if (status != UMI_STATUS_OK) goto dependency_error;
    status = umi_fabric_workflow_plan_build(&scratch->graph, &scratch->candidate.order);
    if (status != UMI_STATUS_OK) goto dependency_error;

    /* Translate canonical IDs back to caller indices without changing order.
     * These indices let a host locate its original typed values and metadata. */
    for (size_t i = 0U; i < scratch->candidate.order.count; ++i)
        for (size_t j = 0U; j < d->node_count; ++j)
            if (strcmp(scratch->candidate.order.step_ids[i], d->nodes[j].node_id) == 0)
                scratch->candidate.source_indices[i] = j;
    *out_plan = scratch->candidate;
    goto cleanup;
dependency_error:
    (void)refuse(diagnostic, status, UMI_AI_WORKFLOW_DEPENDENCY_ERROR, none, none, none);
cleanup:
    free(scratch);
    return status;
}

/* Stable labels are intentionally independent of UI language and caller data;
 * a client may translate them while retaining the numeric diagnostic code. */
const char *umi_ai_workflow_issue_text(UmiAiWorkflowIssue issue)
{
    switch (issue) {
    case UMI_AI_WORKFLOW_VALID: return "structurally valid";
    case UMI_AI_WORKFLOW_BAD_ARGUMENT: return "invalid workflow argument";
    case UMI_AI_WORKFLOW_LIMIT: return "workflow capacity exceeded";
    case UMI_AI_WORKFLOW_BAD_IDENTIFIER: return "invalid identifier";
    case UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER: return "duplicate identifier or output selection";
    case UMI_AI_WORKFLOW_BAD_TYPE: return "invalid registered type or supplied-input mask";
    case UMI_AI_WORKFLOW_BAD_LINK: return "missing node or port";
    case UMI_AI_WORKFLOW_DUPLICATE_BINDING: return "input has more than one binding";
    case UMI_AI_WORKFLOW_TYPE_MISMATCH: return "incompatible port types";
    case UMI_AI_WORKFLOW_CYCLE: return "cyclic dependency";
    case UMI_AI_WORKFLOW_MISSING_INPUT: return "required input is not supplied";
    case UMI_AI_WORKFLOW_EFFECT_DENIED: return "required effect is not approved";
    case UMI_AI_WORKFLOW_DEPENDENCY_ERROR: return "allocation or canonical planner failure";
    default: return "unknown workflow diagnostic";
    }
}
