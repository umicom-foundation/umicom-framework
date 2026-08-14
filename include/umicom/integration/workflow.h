/* Umicom Framework | Integration workflow graph | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_INTEGRATION_WORKFLOW_H
#define UMICOM_INTEGRATION_WORKFLOW_H
#include "umicom/integration/designer_types.h"
typedef struct UmiIntegrationWorkflowNode {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char name[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    UmiIntegrationDesignerNodeKind kind;
    char reference_id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
} UmiIntegrationWorkflowNode;
typedef struct UmiIntegrationWorkflowEdge {
    char from[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char to[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char condition[UMI_INTEGRATION_DESIGNER_TEXT_CAPACITY];
} UmiIntegrationWorkflowEdge;
typedef struct UmiIntegrationWorkflow {
    char id[UMI_INTEGRATION_DESIGNER_ID_CAPACITY];
    char name[UMI_INTEGRATION_DESIGNER_NAME_CAPACITY];
    UmiIntegrationWorkflowNode nodes[UMI_INTEGRATION_DESIGNER_MAX_NODES];
    size_t node_count;
    UmiIntegrationWorkflowEdge edges[UMI_INTEGRATION_DESIGNER_MAX_EDGES];
    size_t edge_count;
    uint64_t revision;
} UmiIntegrationWorkflow;
UmiStatus umi_integration_workflow_init(UmiIntegrationWorkflow *workflow,const char *id,const char *name);
UmiStatus umi_integration_workflow_add_node(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowNode *node);
UmiStatus umi_integration_workflow_connect(UmiIntegrationWorkflow *workflow,const UmiIntegrationWorkflowEdge *edge);
const UmiIntegrationWorkflowNode *umi_integration_workflow_find_node(const UmiIntegrationWorkflow *workflow,const char *id);
UmiStatus umi_integration_workflow_validate(const UmiIntegrationWorkflow *workflow,UmiIntegrationDesignerValidation *validation);
#endif
