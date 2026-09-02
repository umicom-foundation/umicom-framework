/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_workflow.c
 *
 * PURPOSE:
 *   Implement the test integration workflow behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Workflow graph | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiIntegrationWorkflow workflow = umi_test_workflow();
    UmiIntegrationDesignerValidation validation = {0};
    assert(umi_integration_workflow_validate(&workflow,&validation) == UMI_STATUS_OK);
    assert(workflow.node_count == 3U && workflow.edge_count == 2U && workflow.revision == 6U);
    assert(umi_integration_workflow_find_node(&workflow,"request")->kind == UMI_INTEGRATION_DESIGNER_REQUEST);
    return 0;
}
