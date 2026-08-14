/* Umicom Framework | Deterministic workflow simulator | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/simulator.h"
#include <string.h>

static const UmiIntegrationWorkflowNode *trigger_node(const UmiIntegrationWorkflow *workflow)
{
    size_t index;
    for (index = 0U; index < workflow->node_count; ++index) if (workflow->nodes[index].kind == UMI_INTEGRATION_DESIGNER_TRIGGER) return &workflow->nodes[index];
    return NULL;
}

static const UmiIntegrationWorkflowNode *next_node(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    for (index = 0U; index < workflow->edge_count; ++index) if (strcmp(workflow->edges[index].from,node_id) == 0) return umi_integration_workflow_find_node(workflow,workflow->edges[index].to);
    return NULL;
}

UmiStatus umi_integration_simulate(const UmiIntegrationWorkflow *workflow,const UmiIntegrationExecutionPolicy *policy,const char *run_id,UmiIntegrationSimulation *out_simulation)
{
    const UmiIntegrationWorkflowNode *node;
    UmiIntegrationDesignerValidation validation = {0};
    UmiStatus status;
    if (workflow == NULL || policy == NULL || run_id == NULL || out_simulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_workflow_validate(workflow,&validation);
    if (status != UMI_STATUS_OK) return status;
    status = umi_integration_execution_policy_validate(policy,&validation);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_simulation,0,sizeof(*out_simulation));
    status = umi_integration_designer_copy(out_simulation->run_id,sizeof(out_simulation->run_id),run_id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(out_simulation->workflow_id,sizeof(out_simulation->workflow_id),workflow->id);
    if (status != UMI_STATUS_OK) return status;
    out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_PENDING;
    node = trigger_node(workflow);
    while (node != NULL && out_simulation->trace_count < UMI_INTEGRATION_DESIGNER_MAX_TRACE) {
        UmiIntegrationTraceEntry *entry = &out_simulation->trace[out_simulation->trace_count];
        entry->sequence = out_simulation->trace_count + 1U;
        entry->kind = node->kind;
        entry->status = UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED;
        status = umi_integration_designer_copy(entry->node_id,sizeof(entry->node_id),node->id);
        if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(entry->detail,sizeof(entry->detail),node->kind == UMI_INTEGRATION_DESIGNER_REQUEST ? "Request planned without network I/O" : "Node evaluated deterministically");
        if (status != UMI_STATUS_OK) return status;
        out_simulation->trace_count += 1U;
        if (node->kind == UMI_INTEGRATION_DESIGNER_REQUEST) out_simulation->attempted_requests += 1U;
        if (node->kind == UMI_INTEGRATION_DESIGNER_RESPONSE || node->kind == UMI_INTEGRATION_DESIGNER_STOP) {
            out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED;
            return UMI_STATUS_OK;
        }
        node = next_node(workflow,node->id);
    }
    out_simulation->status = UMI_INTEGRATION_DESIGNER_RUN_FAILED;
    return UMI_STATUS_INVALID_STATE;
}
