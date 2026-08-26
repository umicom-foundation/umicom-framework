/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_debounce_policy.c
 *
 * PURPOSE:
 *   Exercise the debounce policy reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/debounce_policy.h"
int main(void) { UmiUiReactiveDebouncePolicy p={50U,0U,0U};if(umi_ui_reactive_debounce_policy_record(&p,100U)!=150U)return 1;return umi_ui_reactive_debounce_policy_due(&p,150U)?0:2; }
