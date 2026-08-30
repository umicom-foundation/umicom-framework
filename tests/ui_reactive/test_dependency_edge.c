/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_edge.c
 *
 * PURPOSE:
 *   Exercise the dependency edge reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_edge.h"
int main(void) { UmiUiReactiveDependencyEdge item; umi_ui_reactive_dependency_edge_init(&item); return umi_ui_reactive_dependency_edge_valid(&item) ? 0 : 1; }
