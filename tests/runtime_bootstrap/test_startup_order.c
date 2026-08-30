/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_startup_order.c
 *
 * PURPOSE:
 *   Implement the test startup order behavior for
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
 * File: tests/runtime_bootstrap/test_startup_order.c
 *
 * PURPOSE:
 *   Focused regression coverage for Derive service start order from the dependency graph.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/graph_edge.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/startup_order.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceGraph g; UmiBootstrapGraphNode a,b; UmiBootstrapGraphEdge e; UmiBootstrapIdList order;
    umi_bootstrap_service_graph_init(&g);
    CHECK(umi_bootstrap_graph_node_init(&a,"svc.a",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_node_init(&b,"svc.b",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&a)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_edge_init(&e,"svc.a","svc.b",true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_edge(&g,&e)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_startup_order(&g,&order)==UMI_STATUS_OK);
    CHECK(order.count==2U);
    return 0;
}
