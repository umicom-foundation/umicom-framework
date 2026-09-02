/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_cycle_report.c
 *
 * PURPOSE:
 *   Implement the test cycle report behavior for
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
 * File: tests/runtime_bootstrap/test_cycle_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for Extract a representative cycle path from a service dependency graph.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/graph_edge.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/cycle_report.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapServiceGraph g; UmiBootstrapGraphNode a,b; UmiBootstrapGraphEdge ab,ba; UmiBootstrapIdList cycle;
    umi_bootstrap_service_graph_init(&g);
    CHECK(umi_bootstrap_graph_node_init(&a,"svc.a",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_node_init(&b,"svc.b",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_edge_init(&ab,"svc.a","svc.b",true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_edge_init(&ba,"svc.b","svc.a",true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_edge(&g,&ab)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_edge(&g,&ba)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_cycle_report(&g,&cycle)==UMI_STATUS_OK);
    CHECK(cycle.count>=3U);
    return 0;
}
