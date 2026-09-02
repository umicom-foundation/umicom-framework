/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_throttle_policy.c
 *
 * PURPOSE:
 *   Exercise the throttle policy reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/throttle_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveThrottlePolicy p={10U,0U};/* Apply this operation only while the related capability or state is available. */ if(!umi_ui_reactive_throttle_policy_admit(&p,100U))return 1;return umi_ui_reactive_throttle_policy_admit(&p,105U)?2:0; }
