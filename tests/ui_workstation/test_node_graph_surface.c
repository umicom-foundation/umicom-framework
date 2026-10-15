/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_node_graph_surface.c
 *
 * PURPOSE:
 *   Implement the test node graph surface behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "../value_archive/construction_cases.h"
#include "umicom/ui/workstation/node_graph_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted node graph surface value. */
static int UmiWsNodeGraphSurfaceConstructionCases(void)
{
    UmiWsNodeGraphSurface *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_node_graph_surface_init(value, "ai.workflow") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->graph_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_node_graph_surface_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_node_graph_surface_init(value, value->graph_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->graph_id, "ai.workflow") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsNodeGraphSurfaceConstructionCases() != 0) return 1;

    UmiWsNodeGraphSurface g;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_node_graph_surface_init(&g, "ai.workflow") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_node_graph_surface_add_node(&g, "prompt", "Prompt", 0.1, 0.2) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_node_graph_surface_add_node(&g, "model", "Model", 0.6, 0.2) != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_node_graph_surface_add_edge(&g, "prompt", "model") != UMI_STATUS_OK) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (g.edge_count != 1U) return 5;
    puts("node graph surface: ok");
    return 0;
}
