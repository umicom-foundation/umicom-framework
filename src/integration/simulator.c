/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/simulator.c
 *
 * PURPOSE:
 *   Implement the simulator behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Deterministic workflow simulator | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/simulator.h"
#include <string.h>

/* Provide the trigger node operation used by this module and its client applications. */
static const UmiIntegrationWorkflowNode *trigger_node(const UmiIntegrationWorkflow *workflow)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->node_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (workflow->nodes[index].kind == UMI_INTEGRATION_DESIGNER_TRIGGER) return &workflow->nodes[index];
    return NULL;
}

/* Provide the next node operation used by this module and its client applications. */
static const UmiIntegrationWorkflowNode *next_node(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->edge_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(workflow->edges[index].from,node_id) == 0) return umi_integration_workflow_find_node(workflow,workflow->edges[index].to);
    return NULL;
}

/*
 * Provide the integration simulate operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_simulate(const UmiIntegrationWorkflow *workflow,const UmiIntegrationExecutionPolicy *policy,const char *run_id,UmiIntegrationSimulation *out_simulation)
{
    const UmiIntegrationWorkflowNode *node;
    UmiIntegrationDesignerValidation validation = {0};
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || policy == NULL || run_id == NULL || out_simulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_workflow_validate(workflow,&validation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_integration_execution_policy_validate(policy,&validation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_simulation,0,sizeof(*out_simulation));
    status = umi_integration_designer_copy(out_simulation->run_id,sizeof(out_simulation->run_id),run_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_simulation->workflow_id,sizeof(out_simulation->workflow_id),workflow->id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_PENDING;
    node = trigger_node(workflow);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (node != NULL && out_simulation->trace_count < UMI_INTEGRATION_DESIGNER_MAX_TRACE) {
        UmiIntegrationTraceEntry *entry = &out_simulation->trace[out_simulation->trace_count];
        entry->sequence = out_simulation->trace_count + 1U;
        entry->kind = node->kind;
        entry->status = UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED;
        status = umi_integration_designer_copy(entry->node_id,sizeof(entry->node_id),node->id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(entry->detail,sizeof(entry->detail),node->kind == UMI_INTEGRATION_DESIGNER_REQUEST ? "Request planned without network I/O" : "Node evaluated deterministically");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_simulation->trace_count += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind == UMI_INTEGRATION_DESIGNER_REQUEST) out_simulation->attempted_requests += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind == UMI_INTEGRATION_DESIGNER_RESPONSE || node->kind == UMI_INTEGRATION_DESIGNER_STOP) {
            out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED;
            return UMI_STATUS_OK;
        }
        node = next_node(workflow,node->id);
    }
    out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_FAILED;
    return UMI_STATUS_INVALID_STATE;
}
