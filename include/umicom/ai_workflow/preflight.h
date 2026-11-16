/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workflow/preflight.h
 *
 * PURPOSE:
 *   Describe and admit typed AI workflows without loading providers or executing effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKFLOW_PREFLIGHT_H
#define UMICOM_AI_WORKFLOW_PREFLIGHT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/workflow_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_WORKFLOW_MAX_TYPES 32U
#define UMI_AI_WORKFLOW_MAX_PORTS 8U
#define UMI_AI_WORKFLOW_MAX_NODES UMI_FABRIC_MAX_STEPS
#define UMI_AI_WORKFLOW_MAX_LINKS UMI_FABRIC_MAX_EDGES
#define UMI_AI_WORKFLOW_ID_CAPACITY UMI_FABRIC_ID_CAPACITY

typedef enum UmiAiWorkflowValueKind {
    UMI_AI_WORKFLOW_TEXT = 1,
    UMI_AI_WORKFLOW_IMAGE,
    UMI_AI_WORKFLOW_VIDEO,
    UMI_AI_WORKFLOW_AUDIO,
    UMI_AI_WORKFLOW_DOCUMENT,
    UMI_AI_WORKFLOW_CONDITIONING,
    UMI_AI_WORKFLOW_LATENT,
    UMI_AI_WORKFLOW_NUMBER,
    UMI_AI_WORKFLOW_BOOLEAN
} UmiAiWorkflowValueKind;

/* These are declared requirements, not capabilities granted to a provider.
 * The eventual executor must enforce resource scopes and current approvals. */
#define UMI_AI_WORKFLOW_EFFECT_READ_FILE UINT32_C(1)
#define UMI_AI_WORKFLOW_EFFECT_WRITE_FILE UINT32_C(2)
#define UMI_AI_WORKFLOW_EFFECT_NETWORK UINT32_C(4)
#define UMI_AI_WORKFLOW_EFFECT_PROCESS UINT32_C(8)
#define UMI_AI_WORKFLOW_KNOWN_EFFECTS UINT32_C(15)

typedef struct UmiAiWorkflowPort {
    char name[UMI_AI_WORKFLOW_ID_CAPACITY];
    UmiAiWorkflowValueKind kind;
    bool required;
} UmiAiWorkflowPort;

typedef struct UmiAiWorkflowNodeType {
    char operation_id[UMI_AI_WORKFLOW_ID_CAPACITY];
    UmiAiWorkflowPort inputs[UMI_AI_WORKFLOW_MAX_PORTS];
    UmiAiWorkflowPort outputs[UMI_AI_WORKFLOW_MAX_PORTS];
    size_t input_count;
    size_t output_count;
    bool available;
    uint32_t effects;
} UmiAiWorkflowNodeType;

typedef struct UmiAiWorkflowNode {
    char node_id[UMI_AI_WORKFLOW_ID_CAPACITY];
    size_t type_index;
    /* Bit p declares an externally supplied input p. It cannot also be linked.
     * This declaration does not validate an actual input value or file. */
    uint64_t supplied_inputs;
} UmiAiWorkflowNode;

typedef struct UmiAiWorkflowLink {
    size_t source_node;
    size_t source_port;
    size_t target_node;
    size_t target_port;
} UmiAiWorkflowLink;

typedef struct UmiAiWorkflowDescription {
    const UmiAiWorkflowNodeType *types;
    size_t type_count;
    const UmiAiWorkflowNode *nodes;
    size_t node_count;
    const UmiAiWorkflowLink *links;
    size_t link_count;
    const size_t *output_nodes;
    size_t output_count;
} UmiAiWorkflowDescription;

typedef enum UmiAiWorkflowIssue {
    UMI_AI_WORKFLOW_ISSUE_NONE = 0,
    UMI_AI_WORKFLOW_ISSUE_ARGUMENT,
    UMI_AI_WORKFLOW_ISSUE_CAPACITY,
    UMI_AI_WORKFLOW_ISSUE_IDENTIFIER,
    UMI_AI_WORKFLOW_ISSUE_DUPLICATE,
    UMI_AI_WORKFLOW_ISSUE_VALUE_KIND,
    UMI_AI_WORKFLOW_ISSUE_NODE_TYPE,
    UMI_AI_WORKFLOW_ISSUE_PORT,
    UMI_AI_WORKFLOW_ISSUE_LINK_NODE,
    UMI_AI_WORKFLOW_ISSUE_TYPE_MISMATCH,
    UMI_AI_WORKFLOW_ISSUE_BINDING,
    UMI_AI_WORKFLOW_ISSUE_REQUIRED_INPUT,
    UMI_AI_WORKFLOW_ISSUE_CYCLE,
    UMI_AI_WORKFLOW_ISSUE_OUTPUT,
    UMI_AI_WORKFLOW_ISSUE_EFFECT_DEFINITION,
    UMI_AI_WORKFLOW_ISSUE_EFFECT_DENIED,
    UMI_AI_WORKFLOW_ISSUE_ALLOCATION,
    UMI_AI_WORKFLOW_ISSUE_GRAPH
} UmiAiWorkflowIssue;

typedef struct UmiAiWorkflowDiagnostic {
    UmiAiWorkflowIssue issue;
    /* SIZE_MAX means that a field does not identify an affected item. */
    size_t node_index;
    size_t link_index;
    size_t port_index;
    size_t type_index;
} UmiAiWorkflowDiagnostic;

typedef struct UmiAiWorkflowPlan {
    UmiFabricWorkflowPlan order;
    size_t node_indices[UMI_AI_WORKFLOW_MAX_NODES];
    size_t unavailable_indices[UMI_AI_WORKFLOW_MAX_NODES];
    size_t unavailable_count;
    uint32_t required_effects;
    bool execution_ready;
} UmiAiWorkflowPlan;

/* Validate every live schema, node, link and output. Cycles anywhere are refused.
 * Plan only selected outputs and their dependency closure. Required inputs,
 * allowed effects and unavailable providers apply to that selected closure.
 * Ready nodes use the canonical Framework lexical ordering; unavailable_indices
 * and node_indices contain original description indices in execution order.
 *
 * Identifiers are nonempty terminated ASCII letters, digits, underscore, dot
 * or hyphen. Counts must fit the published limits; unused fixed slots are ignored.
 * A missing provider yields OK with execution_ready=false, not synthetic output.
 * execution_ready is based only on host declarations, not runtime qualification.
 *
 * All arguments are required. Borrowed arrays remain readable for the call.
 * Input arrays, description, output and diagnostic must not overlap, and callers
 * serialize access. Any failure preserves every byte of out_plan; diagnostic is
 * replaced. Temporary allocation may fail. No file, network, model or process
 * action occurs. The order fingerprint is not a full workflow/cache identity
 * or permission token; recheck settings, resource scopes and provider identity
 * when a separate executor accepts a job. */
UmiStatus umi_ai_workflow_preflight(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowPlan *out_plan,
    UmiAiWorkflowDiagnostic *diagnostic);
const char *umi_ai_workflow_issue_text(UmiAiWorkflowIssue issue);

#ifdef __cplusplus
}
#endif
#endif
