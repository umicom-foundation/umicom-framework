/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/workflow.c
 *
 * PURPOSE:
 *   Implement the workflow behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration workflow graph | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/workflow.h"
#include <string.h>

/*
 * Initialise integration workflow from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_integration_workflow_init(UmiIntegrationWorkflow *workflow,const char *id,const char *name)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(workflow,0,sizeof(*workflow));
    status = umi_integration_designer_copy(workflow->id,sizeof(workflow->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(workflow->name,sizeof(workflow->name),name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workflow->revision = 1U;
    return status;
}

/*
 * Provide the integration workflow find node operation used by this module and its client
 * applications.
 */
const UmiIntegrationWorkflowNode *umi_integration_workflow_find_node(const UmiIntegrationWorkflow *workflow,const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->node_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(workflow->nodes[index].id,id) == 0) return &workflow->nodes[index];
    return NULL;
}

/*
 * Provide the integration workflow add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_workflow_add_node(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowNode *node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || node == NULL || node->id[0] == '\0' || node->kind < UMI_INTEGRATION_DESIGNER_TRIGGER || node->kind > UMI_INTEGRATION_DESIGNER_STOP) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_workflow_find_node(workflow,node->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workflow->node_count >= UMI_INTEGRATION_DESIGNER_MAX_NODES) return UMI_STATUS_CAPACITY_EXCEEDED;
    workflow->nodes[workflow->node_count] = *node;
    workflow->node_count += 1U;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration workflow connect operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_workflow_connect(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || edge == NULL || edge->from[0] == '\0' || edge->to[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strcmp(edge->from,edge->to) == 0) return UMI_STATUS_INVALID_STATE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_workflow_find_node(workflow,edge->from) == NULL || umi_integration_workflow_find_node(workflow,edge->to) == NULL) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workflow->edge_count >= UMI_INTEGRATION_DESIGNER_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED;
    workflow->edges[workflow->edge_count] = *edge;
    workflow->edge_count += 1U;
    workflow->revision += 1U;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by incoming without changing their state. */
static size_t incoming_count(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->edge_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(workflow->edges[index].to,node_id) == 0) count += 1U;
    return count;
}

/* Return the number of records represented by outgoing without changing their state. */
static size_t outgoing_count(const UmiIntegrationWorkflow *workflow,const char *node_id)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->edge_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(workflow->edges[index].from,node_id) == 0) count += 1U;
    return count;
}

/*
 * Check that integration workflow satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_workflow_validate(const UmiIntegrationWorkflow *workflow,UmiIntegrationDesignerValidation *validation)
{
    size_t index;
    size_t triggers = 0U;
    size_t terminals = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workflow == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (workflow->node_count == 0U) (void)umi_integration_designer_validation_add(validation,"workflow.nodes","Workflow must contain nodes");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workflow->node_count; ++index) {
        const UmiIntegrationWorkflowNode *node = &workflow->nodes[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind == UMI_INTEGRATION_DESIGNER_TRIGGER) {
            triggers += 1U;
            /* Apply this operation only while the related capability or state is available. */
            if (incoming_count(workflow,node->id) != 0U) (void)umi_integration_designer_validation_add(validation,node->id,"Trigger cannot have incoming edges");
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (node->kind == UMI_INTEGRATION_DESIGNER_RESPONSE || node->kind == UMI_INTEGRATION_DESIGNER_STOP) {
            terminals += 1U;
            /* Apply this operation only while the related capability or state is available. */
            if (outgoing_count(workflow,node->id) != 0U) (void)umi_integration_designer_validation_add(validation,node->id,"Terminal node cannot have outgoing edges");
        } else /* Apply this branch only when its contract condition is satisfied. */ if (outgoing_count(workflow,node->id) == 0U) {
            (void)umi_integration_designer_validation_add(validation,node->id,"Non-terminal node requires an outgoing edge");
        }
    }
    /* Apply this operation only while the related capability or state is available. */
    if (triggers != 1U) (void)umi_integration_designer_validation_add(validation,"workflow.trigger","Workflow requires exactly one trigger");
    /* Apply this operation only while the related capability or state is available. */
    if (terminals == 0U) (void)umi_integration_designer_validation_add(validation,"workflow.terminal","Workflow requires a response or stop node");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
