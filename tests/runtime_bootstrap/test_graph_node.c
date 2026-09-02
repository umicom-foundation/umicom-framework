/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_graph_node.c
 *
 * PURPOSE:
 *   Implement the test graph node behavior for
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
 * File: tests/runtime_bootstrap/test_graph_node.c
 *
 * PURPOSE:
 *   Focused regression coverage for Construct service-graph nodes with explicit priority and enabled state.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/graph_node.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiBootstrapGraphNode n;
    CHECK(umi_bootstrap_graph_node_init(&n, "svc.alpha", 5, true) == UMI_STATUS_OK);
    CHECK(n.enabled && n.priority == 5);
    return 0;
}
