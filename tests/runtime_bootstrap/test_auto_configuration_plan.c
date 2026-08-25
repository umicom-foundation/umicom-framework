/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_auto_configuration_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for Select and priority-order applicable auto-configurations.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/auto_configuration.h"
#include "umicom/runtime/bootstrap/auto_configuration_catalogue.h"
#include "umicom/runtime/bootstrap/auto_configuration_plan.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapAutoConfiguration a,b; UmiBootstrapAutoConfigurationCatalogue cat;
    UmiBootstrapAutoConfigurationPlan plan; UmiBootstrapPropertySet props={0},env={0}; UmiBootstrapIdList f={0},c={0};
    umi_bootstrap_auto_configuration_catalogue_init(&cat);
    CHECK(umi_bootstrap_auto_configuration_init(&a,"autoconfig.low",1,UMI_BOOTSTRAP_CONDITION_ALL)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_init(&b,"autoconfig.high",9,UMI_BOOTSTRAP_CONDITION_ALL)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_catalogue_add(&cat,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_catalogue_add(&cat,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_auto_configuration_plan_build(&cat,&props,&env,&f,&c,"windows",&plan)==UMI_STATUS_OK);
    CHECK(plan.count==2U && strcmp(plan.configuration_ids[0],"autoconfig.high")==0);
    return 0;
}
