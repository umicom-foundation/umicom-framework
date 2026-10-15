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
#include "../value_archive/construction_cases.h"
#include <string.h>
#include "umicom/runtime/bootstrap/bootstrap_context.h"
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/starter_catalogue.h"
#include "umicom/runtime/bootstrap/auto_configuration_catalogue.h"
#include "umicom/runtime/bootstrap/bootstrap_service.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The previous automatic-storage fixture is retained for review. Its large
 * aggregate values can exhaust a native thread stack before the first check.
 * The replacement owns those same records on the heap, checks allocation and
 * releases them after the checks, including a failed assertion path. */
#if 0

/* A rejected replacement must preserve the previously accepted graph node value. */


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

#endif

#include <stdio.h>
#include <stdlib.h>

/* Auto-configuration embeds many condition sets. The fixture owns every input
 * and output through one heap allocation rather than enlarging platform stacks. */
typedef struct BootstrapServiceFixture {
    UmiBootstrapContext context;
    UmiBootstrapServiceGraph graph;
    UmiBootstrapStarterCatalogue starters;
    UmiBootstrapAutoConfigurationCatalogue configurations;
    UmiBootstrapPropertySet environment;
    UmiBootstrapIdList features, capabilities;
    UmiBootstrapPlan plan;
    UmiBootstrapAutoConfigurationPlan configuration_plan;
    UmiBootstrapIssueReport issues;
} BootstrapServiceFixture;

static int CheckBootstrapService(BootstrapServiceFixture *fixture)
{
    UmiBootstrapGraphNode node;
    CHECK(umi_bootstrap_context_init(&fixture->context, "app.studio", "windows", true) == UMI_STATUS_OK);
    umi_bootstrap_service_graph_init(&fixture->graph);
    CHECK(umi_bootstrap_graph_node_init(&node, "svc.runtime", 0, true) == UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&fixture->graph, &node) == UMI_STATUS_OK);
    umi_bootstrap_starter_catalogue_init(&fixture->starters);
    umi_bootstrap_auto_configuration_catalogue_init(&fixture->configurations);
    CHECK(umi_bootstrap_service_prepare(&fixture->context, &fixture->graph,
        &fixture->starters, &fixture->configurations, &fixture->environment,
        &fixture->features, &fixture->capabilities, &fixture->plan,
        &fixture->configuration_plan, &fixture->issues) == UMI_STATUS_OK);
    CHECK(fixture->plan.count == 6U && fixture->context.resolved_service_count == 1U);
    return 0;
}

/* Validate constructor refusal before the active service scenario. */
static int UmiBootstrapGraphNodeConstructionCases(void)
{
    UmiBootstrapGraphNode *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_node_init(value, "svc.runtime", 0, true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_node_init(value, oversized, 0, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_node_init(value, value->id, 0, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "svc.runtime") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiBootstrapGraphNodeConstructionCases() != 0) return 1;

    BootstrapServiceFixture *fixture = calloc(1U, sizeof *fixture);
    if (fixture == NULL) { fputs("Cannot allocate bootstrap fixture\n", stderr); return 1; }
    int result = CheckBootstrapService(fixture);
    free(fixture);
    return result;
}
