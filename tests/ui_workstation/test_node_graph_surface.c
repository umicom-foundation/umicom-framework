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
#include "umicom/ui/workstation/node_graph_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
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
