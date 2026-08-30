/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_node.c
 *
 * PURPOSE:
 *   Exercise the dependency node reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_node.h"
int main(void) { UmiUiReactiveDependencyNode item; umi_ui_reactive_dependency_node_init(&item); return umi_ui_reactive_dependency_node_valid(&item) ? 0 : 1; }
