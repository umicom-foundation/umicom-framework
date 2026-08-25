#include <stdio.h>
#include "umicom/ui/workstation/node_graph_surface.h"
int main(void) {
    UmiWsNodeGraphSurface g;
    if (umi_ws_node_graph_surface_init(&g, "ai.workflow") != UMI_STATUS_OK) return 1;
    if (umi_ws_node_graph_surface_add_node(&g, "prompt", "Prompt", 0.1, 0.2) != UMI_STATUS_OK) return 2;
    if (umi_ws_node_graph_surface_add_node(&g, "model", "Model", 0.6, 0.2) != UMI_STATUS_OK) return 3;
    if (umi_ws_node_graph_surface_add_edge(&g, "prompt", "model") != UMI_STATUS_OK) return 4;
    if (g.edge_count != 1U) return 5;
    puts("node graph surface: ok");
    return 0;
}
