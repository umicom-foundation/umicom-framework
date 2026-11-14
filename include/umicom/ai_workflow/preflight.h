/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workflow/preflight.h
 *
 * PURPOSE:
 *   Validate typed AI workflow descriptions before any execution is authorised.
 *
 * EDUCATIONAL OVERVIEW:
 *   Port contracts describe data, not tensors. Dependency ordering is delegated
 *   to the existing Integration Fabric; this module never runs a model.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT for this original Umicom contribution. No ComfyUI source is included.
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_WORKFLOW_PREFLIGHT_H
#define UMICOM_AI_WORKFLOW_PREFLIGHT_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/integration/fabric/workflow_plan.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_WORKFLOW_MAX_PORTS 8U
#define UMI_AI_WORKFLOW_MAX_TYPES UMI_FABRIC_MAX_STEPS
#define UMI_AI_WORKFLOW_NO_INDEX SIZE_MAX

/* A type is a semantic contract. IMAGE and LATENT are deliberately different;
 * sharing a byte buffer representation never makes them interchangeable. */
typedef enum UmiAiWorkflowValueKind {
    UMI_AI_WORKFLOW_TEXT = 1,
    UMI_AI_WORKFLOW_MODEL,
    UMI_AI_WORKFLOW_CONDITIONING,
    UMI_AI_WORKFLOW_LATENT,
    UMI_AI_WORKFLOW_IMAGE,
    UMI_AI_WORKFLOW_MASK,
    UMI_AI_WORKFLOW_AUDIO,
    UMI_AI_WORKFLOW_VIDEO,
    UMI_AI_WORKFLOW_INTEGER,
    UMI_AI_WORKFLOW_REAL,
    UMI_AI_WORKFLOW_BOOLEAN,
    UMI_AI_WORKFLOW_ASSET
} UmiAiWorkflowValueKind;

/* Effects describe operations needing host approval. This module performs none
 * of these effects. An admitted plan is not an authentication credential. */
#define UMI_AI_WORKFLOW_READ_ASSET UINT32_C(1)
#define UMI_AI_WORKFLOW_WRITE_ASSET UINT32_C(2)
#define UMI_AI_WORKFLOW_NETWORK UINT32_C(4)
#define UMI_AI_WORKFLOW_DEVICE UINT32_C(8)
#define UMI_AI_WORKFLOW_ALL_EFFECTS UINT32_C(15)

typedef struct UmiAiWorkflowPort {
    char name[UMI_FABRIC_ID_CAPACITY];
    UmiAiWorkflowValueKind kind;
    bool required; /* Applies only to inputs. */
} UmiAiWorkflowPort;

/* The trusted host registry supplies these definitions, NOT an imported graph.
 * available reports a host capability snapshot, not a probe performed here. */
typedef struct UmiAiWorkflowNodeType {
    char operation_id[UMI_FABRIC_ID_CAPACITY];
    UmiAiWorkflowPort inputs[UMI_AI_WORKFLOW_MAX_PORTS];
    UmiAiWorkflowPort outputs[UMI_AI_WORKFLOW_MAX_PORTS];
    size_t input_count;
    size_t output_count;
    uint32_t required_effects;
    bool available;
} UmiAiWorkflowNodeType;

/* A supplied-input bit means the host has already validated an external value.
 * It does not contain or validate media bytes, paths, dimensions or model hashes. */
typedef struct UmiAiWorkflowNode {
    char node_id[UMI_FABRIC_ID_CAPACITY];
    size_t type_index;
    uint32_t supplied_inputs;
} UmiAiWorkflowNode;

typedef struct UmiAiWorkflowLink {
    size_t source_node;
    size_t source_port;
    size_t target_node;
    size_t target_port;
} UmiAiWorkflowLink;

/* Borrow all arrays for the call. The caller serialises access and provides
 * readable, correctly aligned objects for the stated counts. No pointer is
 * retained. Requested output NODE indices select their complete dependency
 * closure; they are not output-port indices. All graph structure is checked. */
typedef struct UmiAiWorkflowDescription {
    const UmiAiWorkflowNodeType *types;
    size_t type_count;
    const UmiAiWorkflowNode *nodes;
    size_t node_count;
    const UmiAiWorkflowLink *links;
    size_t link_count;
    const size_t *requested_outputs;
    size_t requested_output_count;
} UmiAiWorkflowDescription;

typedef enum UmiAiWorkflowIssue {
    UMI_AI_WORKFLOW_VALID = 0,
    UMI_AI_WORKFLOW_BAD_ARGUMENT,
    UMI_AI_WORKFLOW_LIMIT,
    UMI_AI_WORKFLOW_BAD_IDENTIFIER,
    UMI_AI_WORKFLOW_DUPLICATE_IDENTIFIER,
    UMI_AI_WORKFLOW_BAD_TYPE,
    UMI_AI_WORKFLOW_BAD_LINK,
    UMI_AI_WORKFLOW_DUPLICATE_BINDING,
    UMI_AI_WORKFLOW_TYPE_MISMATCH,
    UMI_AI_WORKFLOW_CYCLE,
    UMI_AI_WORKFLOW_MISSING_INPUT,
    UMI_AI_WORKFLOW_EFFECT_DENIED,
    UMI_AI_WORKFLOW_DEPENDENCY_ERROR
} UmiAiWorkflowIssue;

typedef struct UmiAiWorkflowDiagnostic {
    UmiAiWorkflowIssue issue;
    size_t node_index;
    size_t link_index;
    size_t port_index;
} UmiAiWorkflowDiagnostic;

/* Only fully validated plans are published. order.fingerprint is the existing
 * Fabric ordering fingerprint: it is NOT a workflow cache key, model identity,
 * security digest or proof of reproducible inference. Unavailable providers
 * yield a valid structural plan with execution_ready=false, never fake output. */
typedef struct UmiAiWorkflowPlan {
    UmiFabricWorkflowPlan order;
    size_t source_indices[UMI_FABRIC_MAX_STEPS];
    size_t omitted_count;
    size_t unavailable_count;
    size_t unavailable_indices[UMI_FABRIC_MAX_STEPS];
    uint32_t required_effects;
    bool execution_ready;
} UmiAiWorkflowPlan;

/* Validate and plan without filesystem, network, GPU or callback activity.
 * Every non-OK return preserves every byte of out_plan. Optional diagnostic is
 * replaced on every call. Inputs, outputs and diagnostic must not overlap.
 * Missing required inputs and unavailable providers are checked only in the
 * requested dependency closure; invalid links and cycles anywhere are refused.
 * Heap allocation is bounded by Framework graph capacities. */
UmiStatus umi_ai_workflow_preflight(const UmiAiWorkflowDescription *description,
    uint32_t allowed_effects, UmiAiWorkflowPlan *out_plan,
    UmiAiWorkflowDiagnostic *diagnostic);

/* Return a fixed, non-sensitive explanation; no input text is interpolated. */
const char *umi_ai_workflow_issue_text(UmiAiWorkflowIssue issue);

#ifdef __cplusplus
}
#endif
#endif
