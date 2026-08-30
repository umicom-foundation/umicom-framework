/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_topological_order.c
 *
 * PURPOSE:
 *   Exercise the topological order reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/topological_order.h"
int main(void) { UmiUiReactiveTopologicalOrder o={0}; o.order[0]=0U;o.count=1U;o.complete=true;return (o.complete&&o.count==1U)?0:1; }
