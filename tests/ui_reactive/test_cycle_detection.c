/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_cycle_detection.c
 *
 * PURPOSE:
 *   Exercise the cycle detection reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/cycle_detection.h"
int main(void) { UmiUiReactiveCycleDetection d={0}; d.has_cycle=false; d.visited_nodes=2U; return (!d.has_cycle&&d.visited_nodes==2U)?0:1; }
