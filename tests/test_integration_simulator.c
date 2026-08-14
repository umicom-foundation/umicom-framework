/* Umicom Framework Tests | Workflow simulator | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiIntegrationWorkflow workflow = umi_test_workflow();
    UmiIntegrationExecutionPolicy policy;
    UmiIntegrationSimulation simulation;
    umi_integration_execution_policy_default(&policy);
    assert(umi_integration_simulate(&workflow,&policy,"run-54",&simulation) == UMI_STATUS_OK);
    assert(simulation.status == UMI_INTEGRATION_DESIGNER_RUN_SUCCEEDED);
    assert(simulation.trace_count == 3U && simulation.attempted_requests == 1U);
    assert(umi_integration_execution_policy_backoff(&policy,3U) == 1000U);
    return 0;
}
