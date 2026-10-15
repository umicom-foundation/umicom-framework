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
#include "../value_archive/construction_cases.h"
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

/* A rejected replacement must preserve the previously accepted graph edge value. */
static int UmiBootstrapGraphEdgeConstructionCases(void)
{
    UmiBootstrapGraphEdge *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_edge_init(value, "svc.a", "svc.b", true) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->dependency_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_edge_init(value, oversized, "svc.b", true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_edge_init(value, value->dependency_id, "svc.b", true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->dependency_id, "svc.a") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->dependent_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_edge_init(value, "svc.a", oversized, true) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_bootstrap_graph_edge_init(value, "svc.a", value->dependent_id, true) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->dependent_id, "svc.b") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiBootstrapGraphEdgeConstructionCases() != 0) return 1;


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
