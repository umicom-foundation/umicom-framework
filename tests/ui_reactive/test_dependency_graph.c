/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_graph.c
 *
 * PURPOSE:
 *   Exercise the dependency graph reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_graph.h"
int main(void) { UmiUiReactiveDependencyGraph g; uint16_t a=0,b=0; umi_ui_reactive_dependency_graph_init(&g); if(umi_ui_reactive_dependency_graph_add_node(&g,"a",&a)!=UMI_STATUS_OK) return 1; if(umi_ui_reactive_dependency_graph_add_node(&g,"b",&b)!=UMI_STATUS_OK) return 2; return umi_ui_reactive_dependency_graph_add_edge(&g,a,b)==UMI_STATUS_OK?0:3; }
