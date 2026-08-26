/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_required_rule.c
 *
 * PURPOSE:
 *   Exercise the required rule reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/required_rule.h"
int main(void) { UmiUiReactiveRequiredRule item; umi_ui_reactive_required_rule_init(&item); return umi_ui_reactive_required_rule_valid(&item) ? 0 : 1; }
