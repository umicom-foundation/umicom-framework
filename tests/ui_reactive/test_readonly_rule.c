/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_readonly_rule.c
 *
 * PURPOSE:
 *   Exercise the readonly rule reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/readonly_rule.h"
int main(void) { UmiUiReactiveReadonlyRule item; umi_ui_reactive_readonly_rule_init(&item); return umi_ui_reactive_readonly_rule_valid(&item) ? 0 : 1; }
