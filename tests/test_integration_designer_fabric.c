/* Umicom Framework Tests | Designer fabric | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiIntegrationDesignerFabric fabric;
    UmiIntegrationDesignerSnapshot snapshot;
    UmiIntegrationWorkflow workflow = umi_test_workflow();
    UmiIntegrationExecutionPolicy policy;
    UmiIntegrationSimulation simulation;
    umi_integration_designer_fabric_init(&fabric);
    assert(umi_integration_connection_catalogue_add(&fabric.connections,&(UmiIntegrationConnectionProfile){0}) == UMI_STATUS_INVALID_STATE);
    assert(umi_integration_designer_fabric_add_workflow(&fabric,&workflow) == UMI_STATUS_OK);
    umi_integration_execution_policy_default(&policy);
    assert(umi_integration_simulate(&workflow,&policy,"fabric-run",&simulation) == UMI_STATUS_OK);
    assert(umi_integration_designer_fabric_record_run(&fabric,&simulation) == UMI_STATUS_OK);
    umi_integration_designer_fabric_snapshot(&fabric,&snapshot);
    assert(snapshot.workflows == 1U && snapshot.runs.succeeded == 1U && snapshot.revision == 3U);
    return 0;
}
