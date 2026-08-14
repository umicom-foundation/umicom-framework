/* Umicom Framework | Integration workflow graph | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/workflow.h"
#include <string.h>

UmiStatus umi_integration_workflow_init(UmiIntegrationWorkflow *workflow,const char *id,const char *name)
{
    UmiStatus status;
    if (workflow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(workflow,0,sizeof(*workflow));
    status = umi_integration_designer_copy(workflow->id,sizeof(workflow->id),id);
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(workflow->name,sizeof(workflow->name),name);
    if (status == UMI_STATUS_OK) workflow->revision = 1U;
    return status;
}

const UmiIntegrationWorkflowNode *umi_integration_workflow_find_node(const UmiIntegrationWorkflow *workflow,const char *id)
{
    size_t index;
    if (workflow == NULL || id == NULL) return NULL;
    for (index = 0U; index < workflow->node_count; ++index) if (strcmp(workflow->nodes[index].id,id) == 0) return &workflow->nodes[index];
    return NULL;
}

UmiStatus umi_integration_workflow_add_node(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowNode *node)
{
    if (workflow == NULL || node == NULL || node->id[0] == '\0' || node->kind < UMI_INTEGRATION_DESIGNER_TRIGGER || node->kind > UMI_INTEGRATION_DESIGNER_STOP) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_integration_workflow_find_node(workflow,node->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (workflow->node_count >= UMI_INTEGRATION_DESIGNER_MAX_NODES) return UMI_STATUS_CAPACITY_EXCEEDED;
    workflow->nodes[workflow->node_count] = *node;
    workflow->node_count += 1U;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_workflow_connect(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowEdge *edge)
{
    if (workflow == NULL || edge == NULL || edge->from[0] == '\0' || edge->to[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(edge->from,edge->to) == 0) return UMI_STATUS_INVALID_STATE;
    if (umi_integration_workflow_find_node(workflow,edge->from) == NULL || umi_integration_workflow_find_node(workflow,edge->to) == NULL) return UMI_STATUS_NOT_FOUND;
    if (workflow->edge_count >= UMI_INTEGRATION_DESIGNER_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    workflow->edges[workflow->edge_count] = *edge;
    workflow->edge_count += 1U;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

static size_t incoming_count(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < workflow->edge_count; ++index) if (strcmp(workflow->edges[index].to,node_id) == 0) count += 1U;
    return count;
}

static size_t outgoing_count(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < workflow->edge_count; ++index) if (strcmp(workflow->edges[index].from,node_id) == 0) count += 1U;
    return count;
}

UmiStatus umi_integration_workflow_validate(const UmiIntegrationWorkflow *workflow,UmiIntegrationDesignerValidation *validation)
{
    size_t index;
    size_t triggers = 0U;
    size_t terminals = 0U;
    if (workflow == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    if (workflow->node_count == 0U) (void)umi_integration_designer_validation_add(validation,"workflow.nodes","Workflow must contain nodes");
    for (index = 0U; index < workflow->node_count; ++index) {
        const UmiIntegrationWorkflowNode *node = &workflow->nodes[index];
        if (node->kind == UMI_INTEGRATION_DESIGNER_TRIGGER) {
            triggers += 1U;
            if (incoming_count(workflow,node->id) != 0U) (void)umi_integration_designer_validation_add(validation,node->id,"Trigger cannot have incoming edges");
        }
        if (node->kind == UMI_INTEGRATION_DESIGNER_RESPONSE || node->kind == UMI_INTEGRATION_DESIGNER_STOP) {
            terminals += 1U;
            if (outgoing_count(workflow,node->id) != 0U) (void)umi_integration_designer_validation_add(validation,node->id,"Terminal node cannot have outgoing edges");
        } else if (outgoing_count(workflow,node->id) == 0U) {
            (void)umi_integration_designer_validation_add(validation,node->id,"Non-terminal node requires an outgoing edge");
        }
    }
    if (triggers != 1U) (void)umi_integration_designer_validation_add(validation,"workflow.trigger","Workflow requires exactly one trigger");
    if (terminals == 0U) (void)umi_integration_designer_validation_add(validation,"workflow.terminal","Workflow requires a response or stop node");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
