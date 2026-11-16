/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_workflow/memory_plan.h
 *
 * PURPOSE:
 *   Plan sequential native workflow output lifetimes and declared memory peaks through the shared typed graph validator.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_WORKFLOW_MEMORY_PLAN_H
#define UMICOM_AI_WORKFLOW_MEMORY_PLAN_H
#include "umicom/ai_workflow/preflight.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_WORKFLOW_MAX_VALUES (UMI_AI_WORKFLOW_MAX_NODES * UMI_AI_WORKFLOW_MAX_PORTS)

typedef struct UmiAiWorkflowNodeMemory {
    uint64_t output_bytes[UMI_AI_WORKFLOW_MAX_PORTS];
    uint64_t scratch_bytes;
    uint32_t known_outputs;
    bool scratch_known;
} UmiAiWorkflowNodeMemory;
typedef struct UmiAiWorkflowMemoryPolicy {
    uint64_t resident_bytes;
    uint64_t byte_limit;
    bool resident_known;
} UmiAiWorkflowMemoryPolicy;
typedef struct UmiAiWorkflowValueLifetime {
    size_t node_index;
    size_t port_index;
    size_t first_step;
    size_t last_step;
    uint64_t bytes;
    bool bytes_known;
    bool retained_output;
} UmiAiWorkflowValueLifetime;
typedef struct UmiAiWorkflowMemoryStep {
    size_t node_index;
    uint64_t live_before_bytes;
    uint64_t output_bytes;
    uint64_t scratch_bytes;
    uint64_t peak_bytes;
    uint64_t released_bytes;
    uint64_t live_after_bytes;
} UmiAiWorkflowMemoryStep;
typedef enum UmiAiWorkflowMemoryVerdict {
    UMI_AI_WORKFLOW_MEMORY_INCOMPLETE = 1,
    UMI_AI_WORKFLOW_MEMORY_EXCEEDS_LIMIT,
    UMI_AI_WORKFLOW_MEMORY_WITHIN_DECLARED_LIMIT
} UmiAiWorkflowMemoryVerdict;
typedef struct UmiAiWorkflowMemoryPlan {
    UmiAiWorkflowPlan workflow;
    UmiAiWorkflowValueLifetime values[UMI_AI_WORKFLOW_MAX_VALUES];
    UmiAiWorkflowMemoryStep steps[UMI_AI_WORKFLOW_MAX_NODES];
    size_t value_count;
    size_t peak_step; /* SIZE_MAX when the resident baseline is the only peak. */
    uint64_t peak_bytes;
    uint64_t retained_bytes; /* Selected results only, excluding resident bytes. */
    uint64_t final_live_bytes;
    uint64_t remaining_bytes;
    uint64_t deficit_bytes;
    bool estimates_complete;
    UmiAiWorkflowMemoryVerdict verdict;
} UmiAiWorkflowMemoryPlan;
typedef enum UmiAiWorkflowMemoryIssue {
    UMI_AI_WORKFLOW_MEMORY_ISSUE_NONE = 0,
    UMI_AI_WORKFLOW_MEMORY_ISSUE_ARGUMENT,
    UMI_AI_WORKFLOW_MEMORY_ISSUE_WORKFLOW,
    UMI_AI_WORKFLOW_MEMORY_ISSUE_COST,
    UMI_AI_WORKFLOW_MEMORY_ISSUE_OVERFLOW,
    UMI_AI_WORKFLOW_MEMORY_ISSUE_ALLOCATION
} UmiAiWorkflowMemoryIssue;
typedef struct UmiAiWorkflowMemoryDiagnostic {
    UmiAiWorkflowMemoryIssue issue;
    size_t node_index;
    size_t port_index;
    UmiAiWorkflowDiagnostic workflow;
} UmiAiWorkflowMemoryDiagnostic;

/* Describe one memory address space and one sequential execution of the freshly
 * validated selected graph. Each node's outputs own independent storage: no
 * aliasing, in-place reuse, asynchronous consumers or persistent caches are
 * modelled. A shared output is counted once until its last selected consumer;
 * all outputs of an explicitly selected result node remain retained at the end.
 * An unused non-result output is released after its producer finishes. All
 * outputs and scratch coexist with inputs during a step. last_step is the final
 * plan step for retained values, whose retained_output flag forbids release there.
 *
 * Costs are per original node, not execution position; count must equal the
 * description's node count. All declared output ports need known bits for a
 * complete selected estimate. Unknown costs must be zero; non-output slots and
 * unused mask bits must also be zero. A known zero is valid. Inactive costs are
 * checked for shape but do not affect completeness or peak. Scratch exists only
 * during its step. resident_bytes includes live models, external inputs, runtime
 * overhead and host reserve once for the entire plan. Do not add CPU and VRAM
 * together; call with separately justified per-address-space estimates.
 *
 * INCOMPLETE reports only sums of known components, never a fit claim. Complete
 * estimates produce remaining/deficit against byte_limit, which may be zero.
 * Availability remains separately reported by workflow.execution_ready. Neither
 * verdict nor that flag grants execution permission or predicts an actual RSS
 * peak: allocator overhead, decoder expansion and backend workspace belong in
 * the supplied costs. No payload allocation, effect, model load or measurement
 * occurs. The executor must revalidate current inputs and enforce ownership.
 *
 * Every error preserves all bytes of out_plan and replaces diagnostic. Inputs,
 * output and diagnostic must be separate, stable and valid for the call; callers
 * serialize access. Small bounded planning workspaces may fail allocation and
 * are always released. Output contains copied values, with no borrowed pointers.
 * Order fingerprints retain preflight's limited order-only meaning. */
UmiStatus umi_ai_workflow_memory_plan(const UmiAiWorkflowDescription *description,
    const UmiAiWorkflowNodeMemory *costs, size_t cost_count,
    const UmiAiWorkflowMemoryPolicy *policy, uint32_t allowed_effects,
    UmiAiWorkflowMemoryPlan *out_plan, UmiAiWorkflowMemoryDiagnostic *diagnostic);
#ifdef __cplusplus
}
#endif
#endif
