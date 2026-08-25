/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_missing_dependency.c
 *
 * PURPOSE:
 *   Focused regression coverage for Count required graph dependencies whose service node is absent or disabled.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"
#include "umicom/runtime/bootstrap/graph_edge.h"
#include "umicom/runtime/bootstrap/service_graph.h"
#include "umicom/runtime/bootstrap/missing_dependency.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapServiceGraph g; UmiBootstrapGraphNode b; UmiBootstrapGraphEdge e; char first[128];
    umi_bootstrap_service_graph_init(&g);
    CHECK(umi_bootstrap_graph_node_init(&b,"svc.b",0,true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_node(&g,&b)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_edge_init(&e,"svc.a","svc.b",true)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_service_graph_add_edge(&g,&e)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_graph_missing_dependency_count(&g,first,sizeof(first))==1U);
    CHECK(strcmp(first,"svc.a")==0);
    return 0;
}
