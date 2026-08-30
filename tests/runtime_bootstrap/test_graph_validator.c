/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_graph_validator.c
 *
 * PURPOSE:
 *   Implement the test graph validator behavior for
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
 * File: tests/runtime_bootstrap/test_graph_validator.c
 *
 * PURPOSE:
 *   Focused regression coverage for Validate service graphs for missing nodes, self-dependencies and cycles.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/graph_validator.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceGraph g; UmiBootstrapGraphNode a; UmiBootstrapIssueReport report;
    umi_bootstrap_service_graph_init(&g);
    CHECK(umi_bootstrap_graph_node_init(&a,"svc.a",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_validate(&g,&report)==UMI_STATUS_OK);
    CHECK(report.cycles==0U && report.missing_dependencies==0U);
    return 0;
}
