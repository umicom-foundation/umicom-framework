/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_graph_edge.c
 *
 * PURPOSE:
 *   Focused regression coverage for Construct directed dependency edges for service-graph analysis.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_edge.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapGraphEdge e;
    CHECK(umi_bootstrap_graph_edge_init(&e, "svc.a", "svc.b", true) == UMI_STATUS_OK);
    CHECK(e.required);
    CHECK(umi_bootstrap_graph_edge_init(&e, "svc.a", "svc.a", true) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
