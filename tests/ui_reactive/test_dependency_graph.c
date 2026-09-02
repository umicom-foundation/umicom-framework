/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_graph.c
 *
 * PURPOSE:
 *   Exercise the dependency graph reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveDependencyGraph g; uint16_t a=0,b=0; umi_ui_reactive_dependency_graph_init(&g); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_dependency_graph_add_node(&g,"a",&a)!=UMI_STATUS_OK) return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_dependency_graph_add_node(&g,"b",&b)!=UMI_STATUS_OK) return 2; return umi_ui_reactive_dependency_graph_add_edge(&g,a,b)==UMI_STATUS_OK?0:3; }
