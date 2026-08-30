/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_validation_rule.c
 *
 * PURPOSE:
 *   Exercise the validation rule reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_rule.h"
int main(void) { UmiUiReactiveValidationRule item; umi_ui_reactive_validation_rule_init(&item); return umi_ui_reactive_validation_rule_valid(&item) ? 0 : 1; }
