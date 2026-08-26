/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_dependency_node.c
 *
 * PURPOSE:
 *   Exercise the dependency node reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_node.h"
int main(void) { UmiUiReactiveDependencyNode item; umi_ui_reactive_dependency_node_init(&item); return umi_ui_reactive_dependency_node_valid(&item) ? 0 : 1; }
