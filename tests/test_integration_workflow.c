/* Umicom Framework Tests | Workflow graph | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiIntegrationWorkflow workflow = umi_test_workflow();
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_workflow_validate(&workflow,&validation) == UMI_STATUS_OK);
    assert(workflow.node_count == 3U && workflow.edge_count == 2U && workflow.revision == 6U);
    assert(umi_integration_workflow_find_node(&workflow,"request")->kind == UMI_INTEGRATION_DESIGNER_REQUEST);
    return 0;
}
