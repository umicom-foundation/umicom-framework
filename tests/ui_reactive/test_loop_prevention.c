/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_loop_prevention.c
 *
 * PURPOSE:
 *   Exercise the loop prevention reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/loop_prevention.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveLoopPrevention p;umi_ui_reactive_loop_prevention_init(&p);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_loop_prevention_remember(&p,11U)!=UMI_STATUS_OK)return 1;return umi_ui_reactive_loop_prevention_remember(&p,11U)==UMI_STATUS_ALREADY_EXISTS?0:2; }
