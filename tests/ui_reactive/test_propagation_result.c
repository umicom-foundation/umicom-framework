/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_propagation_result.c
 *
 * PURPOSE:
 *   Exercise the propagation result reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_result.h"
int main(void) { UmiUiReactivePropagationResult item; umi_ui_reactive_propagation_result_init(&item); return umi_ui_reactive_propagation_result_valid(&item) ? 0 : 1; }
