/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_service.c
 *
 * PURPOSE:
 *   Implement the test bootstrap service behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_bootstrap_service.c
 *
 * PURPOSE:
 *   Focused regression coverage for Coordinate graph validation, starter and auto-configuration planning for application launch.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_context.h"
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"
#include "umicom/runtime/bootstrap/auto_configuration_catalogue.h"
#include "umicom/runtime/bootstrap/bootstrap_service.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapContext ctx; UmiBootstrapServiceGraph graph; UmiBootstrapGraphNode node;
    UmiBootstrapStarterCatalogue starters; UmiBootstrapAutoConfigurationCatalogue ac;
    UmiBootstrapPropertySet env={0}; UmiBootstrapIdList features={0},caps={0};
    UmiBootstrapPlan plan; UmiBootstrapAutoConfigurationPlan acplan; UmiBootstrapIssueReport issues;
    CHECK(umi_bootstrap_context_init(&ctx,"app.studio","windows",true)==UMI_STATUS_OK);
    umi_bootstrap_service_graph_init(&graph);
    CHECK(umi_bootstrap_graph_node_init(&node,"svc.runtime",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&graph,&node)==UMI_STATUS_OK);
    umi_bootstrap_starter_catalogue_init(&starters);
    umi_bootstrap_auto_configuration_catalogue_init(&ac);
    CHECK(umi_bootstrap_service_prepare(&ctx,&graph,&starters,&ac,&env,&features,&caps,&plan,&acplan,&issues)==UMI_STATUS_OK);
    CHECK(plan.count==6U && ctx.resolved_service_count==1U);
    return 0;
}
