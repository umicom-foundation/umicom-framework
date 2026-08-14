/* Umicom Framework Tests | Request plan | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "integration_designer_fixture.h"
int main(void)
{
    UmiIntegrationConnectionProfile profile = umi_test_connection();
    UmiIntegrationApiOperation operation = {0};
    UmiIntegrationRequestPlan plan;
    (void)umi_integration_designer_copy(operation.id,sizeof(operation.id),"create");
    (void)umi_integration_designer_copy(operation.method,sizeof(operation.method),"POST");
    (void)umi_integration_designer_copy(operation.path,sizeof(operation.path),"/orders");
    assert(umi_integration_request_plan_build(&profile,&operation,"{}","corr-54",&plan) == UMI_STATUS_OK);
    assert(strcmp(plan.target,"https://api.example.test/orders") == 0);
    assert(strcmp(plan.secret_reference,"vault://integration/orders-key") == 0);
    profile.enabled = false;
    assert(umi_integration_request_plan_build(&profile,&operation,"{}","corr-54",&plan) == UMI_STATUS_UNAVAILABLE);
    return 0;
}
