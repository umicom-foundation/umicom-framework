/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_loop_prevention.c
 *
 * PURPOSE:
 *   Exercise the loop prevention reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/loop_prevention.h"
int main(void) { UmiUiReactiveLoopPrevention p;umi_ui_reactive_loop_prevention_init(&p);if(umi_ui_reactive_loop_prevention_remember(&p,11U)!=UMI_STATUS_OK)return 1;return umi_ui_reactive_loop_prevention_remember(&p,11U)==UMI_STATUS_ALREADY_EXISTS?0:2; }
