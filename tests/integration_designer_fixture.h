/* Umicom Framework Tests | Integration designer fixture | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_TEST_INTEGRATION_DESIGNER_FIXTURE_H
#define UMICOM_TEST_INTEGRATION_DESIGNER_FIXTURE_H
#include <string.h>
#include "umicom/integration/designer.h"
static inline UmiIntegrationConnectionProfile umi_test_connection(void)
{
    UmiIntegrationConnectionProfile profile = {0};
    (void)umi_integration_designer_copy(profile.id,sizeof(profile.id),"orders-api");
    (void)umi_integration_designer_copy(profile.name,sizeof(profile.name),"Orders API");
    profile.transport = UMI_INTEGRATION_DESIGNER_REST;
    (void)umi_integration_designer_copy(profile.base_uri,sizeof(profile.base_uri),"https://api.example.test");
    profile.auth = UMI_INTEGRATION_DESIGNER_AUTH_API_KEY;
    (void)umi_integration_designer_copy(profile.secret_reference,sizeof(profile.secret_reference),"vault://integration/orders-key");
    profile.timeout_ms = 5000U;
    profile.enabled = true;
    return profile;
}
static inline UmiIntegrationWorkflow umi_test_workflow(void)
{
    UmiIntegrationWorkflow workflow;
    UmiIntegrationWorkflowNode trigger = {0};
    UmiIntegrationWorkflowNode request = {0};
    UmiIntegrationWorkflowNode response = {0};
    UmiIntegrationWorkflowEdge first = {0};
    UmiIntegrationWorkflowEdge second = {0};
    (void)umi_integration_workflow_init(&workflow,"order-sync","Order sync");
    (void)umi_integration_designer_copy(trigger.id,sizeof(trigger.id),"trigger"); trigger.kind = UMI_INTEGRATION_DESIGNER_TRIGGER;
    (void)umi_integration_designer_copy(request.id,sizeof(request.id),"request"); request.kind = UMI_INTEGRATION_DESIGNER_REQUEST;
    (void)umi_integration_designer_copy(response.id,sizeof(response.id),"response"); response.kind = UMI_INTEGRATION_DESIGNER_RESPONSE;
    (void)umi_integration_workflow_add_node(&workflow,&trigger);
    (void)umi_integration_workflow_add_node(&workflow,&request);
    (void)umi_integration_workflow_add_node(&workflow,&response);
    (void)umi_integration_designer_copy(first.from,sizeof(first.from),"trigger");
    (void)umi_integration_designer_copy(first.to,sizeof(first.to),"request");
    (void)umi_integration_designer_copy(second.from,sizeof(second.from),"request");
    (void)umi_integration_designer_copy(second.to,sizeof(second.to),"response");
    (void)umi_integration_workflow_connect(&workflow,&first);
    (void)umi_integration_workflow_connect(&workflow,&second);
    return workflow;
}
#endif
