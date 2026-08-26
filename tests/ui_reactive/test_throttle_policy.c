/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_throttle_policy.c
 *
 * PURPOSE:
 *   Exercise the throttle policy reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/throttle_policy.h"
int main(void) { UmiUiReactiveThrottlePolicy p={10U,0U};if(!umi_ui_reactive_throttle_policy_admit(&p,100U))return 1;return umi_ui_reactive_throttle_policy_admit(&p,105U)?2:0; }
