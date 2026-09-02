/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_debounce_policy.c
 *
 * PURPOSE:
 *   Exercise the debounce policy reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/debounce_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveDebouncePolicy p={50U,0U,0U};/* Apply this operation only while the related capability or state is available. */ if(umi_ui_reactive_debounce_policy_record(&p,100U)!=150U)return 1;return umi_ui_reactive_debounce_policy_due(&p,150U)?0:2; }
