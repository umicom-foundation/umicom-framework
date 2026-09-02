/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/workflow.h
 *
 * PURPOSE:
 *   Publish the public workflow contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_WORKFLOW_H
#define UMICOM_INTEGRATION_WORKFLOW_H
#include "umicom/integration/designer_types.h"
/**
 * Represent the integration workflow node data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationWorkflowNode {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char name[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    UmiIntegrationDesignerNodeKind kind;
    char reference_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
} UmiIntegrationWorkflowNode;
/**
 * Represent the integration workflow edge data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationWorkflowEdge {
    char from[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char to[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char condition[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationWorkflowEdge;
/**
 * Represent the integration workflow data shared with callers of this public contract.
 */
typedef struct UmiIntegrationWorkflow {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char name[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    UmiIntegrationWorkflowNode nodes[UMI_INTEGRATION_DESIGNER_MAX_NODES];
    size_t node_count;
    UmiIntegrationWorkflowEdge edges[UMI_INTEGRATION_DESIGNER_MAX_EDGES];
    size_t edge_count;
    uint64_t revision;
} UmiIntegrationWorkflow;
/**
 * Initialise integration workflow from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_integration_workflow_init(UmiIntegrationWorkflow *workflow,const char *id,const char *name);
/**
 * Provide the integration workflow add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_workflow_add_node(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowNode *node);
/**
 * Provide the integration workflow connect operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_workflow_connect(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowEdge *edge);
/**
 * Provide the integration workflow find node operation used by this module and its client
 * applications.
 */
const UmiIntegrationWorkflowNode *umi_integration_workflow_find_node(const UmiIntegrationWorkflow *workflow,const char *id);
/**
 * Check that integration workflow satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_workflow_validate(const UmiIntegrationWorkflow *workflow,UmiIntegrationDesignerValidation *validation);
#endif
