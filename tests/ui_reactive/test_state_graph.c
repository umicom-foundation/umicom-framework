/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_graph.c
 *
 * PURPOSE:
 *   Exercise the state graph reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_graph.h"
int main(void) { UmiUiReactiveStateGraph item; umi_ui_reactive_state_graph_init(&item); return umi_ui_reactive_state_graph_valid(&item) ? 0 : 1; }
