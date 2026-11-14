/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workflow/test_preflight.c
 *
 * PURPOSE:
 *   Test typed workflow admission and preservation using native C calls.
 *
 * EDUCATIONAL OVERVIEW:
 *   No models or network fixtures are needed. Successful tests establish
 *   planning contracts, not neural output quality or complete application parity.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT for this original Umicom contribution. No ComfyUI source is included.
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workflow/preflight.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Assertions remain enabled in Release builds; libc assert would disappear
 * under NDEBUG and make a successful test report meaningless. */
#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); return 1; \
} } while (0)

typedef struct Fixture {
    UmiAiWorkflowNodeType types[4];
    UmiAiWorkflowNode nodes[4];
    UmiAiWorkflowLink links[6];
    size_t outputs[4];
    UmiAiWorkflowDescription description;
} Fixture;

/* A tiny fixture models type flow, not neural inference. Tests can mark the
 * declared host capability unavailable without allocating any real model. */
static void fixture_init(Fixture *f)
{
    memset(f, 0, sizeof(*f));
    (void)snprintf(f->types[0].operation_id, sizeof(f->types[0].operation_id), "source.text");
    f->types[0].output_count = 1U;
    f->types[0].outputs[0] = (UmiAiWorkflowPort){"text", UMI_AI_WORKFLOW_TEXT, false};
    f->types[0].available = true;
    (void)snprintf(f->types[1].operation_id, sizeof(f->types[1].operation_id), "transform.text");
    f->types[1].input_count = f->types[1].output_count = 1U;
    f->types[1].inputs[0] = (UmiAiWorkflowPort){"text", UMI_AI_WORKFLOW_TEXT, true};
    f->types[1].outputs[0] = (UmiAiWorkflowPort){"text", UMI_AI_WORKFLOW_TEXT, false};
    f->types[1].available = true;
    f->nodes[0] = (UmiAiWorkflowNode){"first", 0U, 0U};
    f->nodes[1] = (UmiAiWorkflowNode){"second", 1U, 0U};
    f->links[0] = (UmiAiWorkflowLink){0U, 0U, 1U, 0U};
    f->outputs[0] = 1U;
    f->description = (UmiAiWorkflowDescription){f->types, 2U, f->nodes, 2U,
        f->links, 1U, f->outputs, 1U};
}

/* Run the largest permitted chain against the real canonical graph planner.
 * A heap fixture prevents the capacity test from depending on thread stack size. */
static int capacity_chain(void)
{
    UmiAiWorkflowNodeType type = {0};
    UmiAiWorkflowNode *nodes = calloc(UMI_FABRIC_MAX_STEPS, sizeof(*nodes));
    UmiAiWorkflowLink *links = calloc(UMI_FABRIC_MAX_STEPS, sizeof(*links));
    UmiAiWorkflowPlan *plan = calloc(1U, sizeof(*plan));
    if (nodes == NULL || links == NULL || plan == NULL) {
        free(nodes); free(links); free(plan); return 1;
    }
    (void)snprintf(type.operation_id, sizeof(type.operation_id), "chain.text");
    type.input_count = type.output_count = 1U;
    type.inputs[0] = (UmiAiWorkflowPort){"in", UMI_AI_WORKFLOW_TEXT, false};
    type.outputs[0] = (UmiAiWorkflowPort){"out", UMI_AI_WORKFLOW_TEXT, false};
    type.available = true;
    for (size_t i = 0U; i < UMI_FABRIC_MAX_STEPS; ++i) {
        (void)snprintf(nodes[i].node_id, sizeof(nodes[i].node_id), "node-%02zu", i);
        if (i != 0U) links[i - 1U] = (UmiAiWorkflowLink){i - 1U, 0U, i, 0U};
    }
    size_t output = UMI_FABRIC_MAX_STEPS - 1U;
    UmiAiWorkflowDescription d = {&type, 1U, nodes, UMI_FABRIC_MAX_STEPS,
        links, UMI_FABRIC_MAX_STEPS - 1U, &output, 1U};
    UmiStatus status = umi_ai_workflow_preflight(&d, 0U, plan, NULL);
    bool passed = status == UMI_STATUS_OK && plan->order.count == UMI_FABRIC_MAX_STEPS;
    for (size_t i = 0U; passed && i < UMI_FABRIC_MAX_STEPS; ++i)
        passed = plan->source_indices[i] == i;
    free(nodes); free(links); free(plan);
    return passed ? 0 : 1;
}

/* Select one contract mutation, then verify result, diagnostics and byte-for-byte
 * preservation of inputs. Every failure also checks unchanged output storage. */
static int run_case(const char *name)
{
    Fixture f;
    fixture_init(&f);
    UmiAiWorkflowPlan plan;
    memset(&plan, 0xA5, sizeof(plan));
    unsigned char before[sizeof(plan)];
    memcpy(before, &plan, sizeof(plan));
    UmiAiWorkflowDiagnostic diagnostic = {0};
    UmiStatus expected = UMI_STATUS_OK;
    UmiAiWorkflowIssue issue = UMI_AI_WORKFLOW_VALID;
    uint32_t allowed = 0U;
    bool null_description = false, null_plan = false, null_diagnostic = false;

    /* Each case changes only the invariant named by the CTest entry. */
    /* No mutation: both nodes are admitted in dependency order. */
    if (strcmp(name, "valid_chain") == 0) {
    }
    /* Dependencies take precedence over lexical node names. */
    else if (strcmp(name, "dependency_before_alphabetic") == 0) {
        strcpy(f.nodes[0].node_id, "z-source"); strcpy(f.nodes[1].node_id, "a-target");
    }
    /* A link cannot index a node outside the supplied array. */
    else if (strcmp(name, "missing_node") == 0) {
        f.links[0].source_node = 2U;
        expected = UMI_STATUS_NOT_FOUND; issue = UMI_AI_WORKFLOW_BAD_LINK;
    }
    /* A source-port index must exist in its registered schema. */
    else if (strcmp(name, "missing_source_port") == 0) {
        f.links[0].source_port = 1U;
        expected = UMI_STATUS_NOT_FOUND; issue = UMI_AI_WORKFLOW_BAD_LINK;
    }
    /* A destination-port index must exist before mask calculation. */
    else if (strcmp(name, "missing_target_port") == 0) {
        f.links[0].target_port = 99U;
        expected = UMI_STATUS_NOT_FOUND; issue = UMI_AI_WORKFLOW_BAD_LINK;
    }
    /* An image is not silently reinterpreted as text. */
    else if (strcmp(name, "type_mismatch") == 0) {
        f.types[0].outputs[0].kind = UMI_AI_WORKFLOW_IMAGE;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_TYPE_MISMATCH;
    }
    /* Two identical links must not create a double writer. */
    else if (strcmp(name, "duplicate_binding") == 0) {
        f.links[1] = f.links[0]; f.description.link_count = 2U;
        expected = UMI_STATUS_ALREADY_EXISTS; issue = UMI_AI_WORKFLOW_DUPLICATE_BINDING;
    }
    /* External and graph-supplied values cannot compete. */
    else if (strcmp(name, "external_link_conflict") == 0) {
        f.nodes[1].supplied_inputs = 1U;
        expected = UMI_STATUS_ALREADY_EXISTS; issue = UMI_AI_WORKFLOW_DUPLICATE_BINDING;
    }
    /* Missing required input refuses publication. */
    else if (strcmp(name, "required_input") == 0) {
        f.description.link_count = 0U;
        expected = UMI_STATUS_INVALID_STATE; issue = UMI_AI_WORKFLOW_MISSING_INPUT;
    }
    /* An unbound optional input does not force unused upstream work. */
    else if (strcmp(name, "optional_input") == 0) {
        f.description.link_count = 0U; f.types[1].inputs[0].required = false;
    }
    /* An already validated external input can satisfy a required port. */
    else if (strcmp(name, "external_input") == 0) {
        f.description.link_count = 0U; f.nodes[1].supplied_inputs = 1U;
    }
    /* Direct self-dependency is rejected before canonical planning. */
    else if (strcmp(name, "self_cycle") == 0) {
        f.links[0].source_node = 1U;
        expected = UMI_STATUS_INVALID_STATE; issue = UMI_AI_WORKFLOW_CYCLE;
    }
    /* The canonical cycle detector rejects a two-node loop. */
    else if (strcmp(name, "cycle") == 0) {
        f.types[0].input_count = 1U; f.types[0].inputs[0] = f.types[1].inputs[0]; f.links[1] = (UmiAiWorkflowLink){1U,0U,0U,0U}; f.description.link_count = 2U;
        expected = UMI_STATUS_INVALID_STATE; issue = UMI_AI_WORKFLOW_CYCLE;
    }
    /* Capacity is checked before walking a caller array. */
    else if (strcmp(name, "too_many_nodes") == 0) {
        f.description.node_count = UMI_FABRIC_MAX_STEPS + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; issue = UMI_AI_WORKFLOW_LIMIT;
    }
    /* Excessive link counts cannot drive out-of-bounds iteration. */
    else if (strcmp(name, "too_many_links") == 0) {
        f.description.link_count = UMI_FABRIC_MAX_EDGES + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; issue = UMI_AI_WORKFLOW_LIMIT;
    }
    /* Registry size is bounded before inspecting descriptors. */
    else if (strcmp(name, "too_many_types") == 0) {
        f.description.type_count = UMI_AI_WORKFLOW_MAX_TYPES + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED; issue = UMI_AI_WORKFLOW_LIMIT;
    }
    /* A corrupt port count is refused before accessing ports. */
    else if (strcmp(name, "too_many_ports") == 0) {
        f.types[1].input_count = UMI_AI_WORKFLOW_MAX_PORTS + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    /* An empty graph is not a successful execution plan. */
    else if (strcmp(name, "empty_graph") == 0) {
        f.description.node_count = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* Missing input storage is diagnosed without dereferencing it. */
    else if (strcmp(name, "null_description") == 0) {
        null_description = true;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* Missing output storage is diagnosed, not written. */
    else if (strcmp(name, "null_plan") == 0) {
        null_plan = true;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* Diagnostics are optional on a valid request. */
    else if (strcmp(name, "null_diagnostic") == 0) {
        null_diagnostic = true;
    }
    /* Fixed-size identifiers must contain a terminator. */
    else if (strcmp(name, "unterminated_identifier") == 0) {
        memset(f.nodes[0].node_id, 'x', sizeof(f.nodes[0].node_id));
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_IDENTIFIER;
    }
    /* Paths and shell punctuation are not stable schema IDs. */
    else if (strcmp(name, "unsafe_identifier") == 0) {
        strcpy(f.nodes[0].node_id, "a;command");
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_IDENTIFIER;
    }
    /* Node identity is unique independently of insertion order. */
    else if (strcmp(name, "duplicate_node_id") == 0) {
        strcpy(f.nodes[1].node_id, f.nodes[0].node_id);
        expected = UMI_STATUS_ALREADY_EXISTS; issue = UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER;
    }
    /* Registered operation identity cannot be ambiguous. */
    else if (strcmp(name, "duplicate_operation_id") == 0) {
        strcpy(f.types[1].operation_id, f.types[0].operation_id);
        expected = UMI_STATUS_ALREADY_EXISTS; issue = UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER;
    }
    /* Input names form a unique name space in each schema. */
    else if (strcmp(name, "duplicate_port_name") == 0) {
        f.types[1].input_count = 2U; f.types[1].inputs[1] = f.types[1].inputs[0];
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    /* Unknown port enums are not treated as wildcard compatibility. */
    else if (strcmp(name, "unknown_value_kind") == 0) {
        f.types[1].inputs[0].kind = (UmiAiWorkflowValueKind)999;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    /* An instance must refer to a registered operation. */
    else if (strcmp(name, "unknown_type_index") == 0) {
        f.nodes[1].type_index = 5U;
        expected = UMI_STATUS_NOT_FOUND; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    /* Input bits outside the schema cannot bypass validation. */
    else if (strcmp(name, "invalid_supplied_mask") == 0) {
        f.nodes[1].supplied_inputs = 128U;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    /* Network use is refused until explicitly permitted by the host. */
    else if (strcmp(name, "effect_denied") == 0) {
        f.types[1].required_effects = UMI_AI_WORKFLOW_NETWORK;
        expected = UMI_STATUS_PERMISSION_DENIED; issue = UMI_AI_WORKFLOW_EFFECT_DENIED;
    }
    /* An approved effect is recorded without actually performing it. */
    else if (strcmp(name, "effect_allowed") == 0) {
        f.types[1].required_effects = UMI_AI_WORKFLOW_NETWORK; allowed = UMI_AI_WORKFLOW_NETWORK;
    }
    /* A future policy bit requires an explicit API extension. */
    else if (strcmp(name, "unknown_policy_bit") == 0) {
        allowed = UINT32_C(256);
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* A valid schema cannot pretend that a missing engine exists. */
    else if (strcmp(name, "unavailable_provider") == 0) {
        f.types[1].available = false;
    }
    /* A dormant branch does not require its provider to be installed. */
    else if (strcmp(name, "inactive_unavailable") == 0) {
        f.types[2] = f.types[0]; strcpy(f.types[2].operation_id, "unused.type"); f.types[2].available = false; f.description.type_count = 3U; f.nodes[2] = (UmiAiWorkflowNode){"unused",2U,0U}; f.description.node_count = 3U;
    }
    /* Unselected branches may retain incomplete editable inputs. */
    else if (strcmp(name, "inactive_required_input") == 0) {
        f.nodes[2] = (UmiAiWorkflowNode){"unused",1U,0U}; f.description.node_count = 3U;
    }
    /* Requested output indices are checked before reachability. */
    else if (strcmp(name, "invalid_output_index") == 0) {
        f.outputs[0] = 7U;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* Duplicate requested roots are refused rather than double-counted. */
    else if (strcmp(name, "duplicate_output") == 0) {
        f.outputs[1] = 1U; f.description.requested_output_count = 2U;
        expected = UMI_STATUS_ALREADY_EXISTS; issue = UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER;
    }
    /* The selected subgraph is the union of output dependencies. */
    else if (strcmp(name, "multiple_outputs") == 0) {
        f.nodes[2] = (UmiAiWorkflowNode){"third",0U,0U}; f.description.node_count = 3U; f.outputs[1] = 2U; f.description.requested_output_count = 2U;
    }
    /* Distinct data ports share one dependency-ordering edge. */
    else if (strcmp(name, "parallel_port_links") == 0) {
        f.types[0].output_count = 2U; f.types[0].outputs[1] = (UmiAiWorkflowPort){"extra",UMI_AI_WORKFLOW_TEXT,false}; f.types[1].input_count = 2U; f.types[1].inputs[1] = (UmiAiWorkflowPort){"extra",UMI_AI_WORKFLOW_TEXT,true}; f.links[1] = (UmiAiWorkflowLink){0U,1U,1U,1U}; f.description.link_count = 2U;
    }
    /* Structural corruption is rejected even outside requested output closure. */
    else if (strcmp(name, "unselected_cycle") == 0) {
        f.types[0].input_count = 1U; f.types[0].inputs[0] = (UmiAiWorkflowPort){"in",UMI_AI_WORKFLOW_TEXT,false}; f.nodes[2] = (UmiAiWorkflowNode){"third",0U,0U}; f.nodes[3] = (UmiAiWorkflowNode){"fourth",0U,0U}; f.description.node_count=4U; f.links[1]=(UmiAiWorkflowLink){2U,0U,3U,0U}; f.links[2]=(UmiAiWorkflowLink){3U,0U,2U,0U}; f.description.link_count=3U;
        expected = UMI_STATUS_INVALID_STATE; issue = UMI_AI_WORKFLOW_CYCLE;
    }
    /* Independent selected nodes use the canonical lexical tie-break. */
    else if (strcmp(name, "deterministic_order") == 0) {
        f.description.link_count=0U; f.types[1].inputs[0].required=false; strcpy(f.nodes[0].node_id,"z"); strcpy(f.nodes[1].node_id,"a"); f.outputs[1]=0U; f.description.requested_output_count=2U;
    }
    /* A nonzero count requires actual link storage. */
    else if (strcmp(name, "null_link_storage") == 0) {
        f.description.links=NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* Nothing is silently executed when no result was requested. */
    else if (strcmp(name, "zero_outputs") == 0) {
        f.description.requested_output_count=0U;
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_ARGUMENT;
    }
    /* An unsupported effect in a node definition cannot be authorised. */
    else if (strcmp(name, "unknown_effect_definition") == 0) {
        f.types[0].required_effects=UINT32_C(256);
        expected = UMI_STATUS_INVALID_ARGUMENT; issue = UMI_AI_WORKFLOW_BAD_TYPE;
    }
    else { fprintf(stderr, "Unknown case: %s\n", name); return 2; }

    /* Snapshot after the deliberate mutation; the function under test must not
     * edit borrowed definitions or graph data on either success or refusal. */
    unsigned char snapshot[sizeof(f)];
    memcpy(snapshot, &f, sizeof(f));
    UmiStatus status = umi_ai_workflow_preflight(null_description ? NULL : &f.description,
        allowed, null_plan ? NULL : &plan, null_diagnostic ? NULL : &diagnostic);
    CHECK(status == expected);
    CHECK(memcmp(snapshot, &f, sizeof(f)) == 0);
    if (!null_diagnostic) CHECK(diagnostic.issue == issue);
    if (status != UMI_STATUS_OK) CHECK(memcmp(before, &plan, sizeof(plan)) == 0);
    else {
        CHECK(plan.order.count >= 1U);
        CHECK(plan.execution_ready == (strcmp(name, "unavailable_provider") != 0));
        if (strcmp(name, "unavailable_provider") == 0) {
            CHECK(plan.unavailable_count == 1U); CHECK(plan.unavailable_indices[0] == 1U);
        }
        if (strcmp(name, "optional_input") == 0 || strcmp(name, "external_input") == 0) {
            CHECK(plan.order.count == 1U); CHECK(plan.omitted_count == 1U);
        }
        if (strcmp(name, "inactive_unavailable") == 0 || strcmp(name, "inactive_required_input") == 0) {
            CHECK(plan.order.count == 2U); CHECK(plan.omitted_count == 1U);
        }
        if (strcmp(name, "multiple_outputs") == 0) CHECK(plan.order.count == 3U);
        if (strcmp(name, "deterministic_order") == 0) CHECK(strcmp(plan.order.step_ids[0], "a") == 0);
        if (strcmp(name, "dependency_before_alphabetic") == 0)
            CHECK(strcmp(plan.order.step_ids[0], "z-source") == 0);
        if (strcmp(name, "effect_allowed") == 0) CHECK(plan.required_effects == UMI_AI_WORKFLOW_NETWORK);
        if (strcmp(name, "valid_chain") == 0 || strcmp(name, "parallel_port_links") == 0) {
            CHECK(plan.order.count == 2U);
            CHECK(plan.source_indices[0] == 0U); CHECK(plan.source_indices[1] == 1U);
        }
    }
    CHECK(umi_ai_workflow_issue_text(issue) != NULL);
    CHECK(strcmp(umi_ai_workflow_issue_text((UmiAiWorkflowIssue)999), "unknown workflow diagnostic") == 0);
    return 0;
}

/* CTest launches each named case separately so a crash cannot hide later cases.
 * The capacity fixture has its own allocation/cleanup path and no model data. */
int main(int argc, char **argv)
{
    if (argc != 2) { fputs("Pass one registered test-case name.\n", stderr); return 2; }
    if (strcmp(argv[1], "capacity_chain") == 0) return capacity_chain();
    return run_case(argv[1]);
}
