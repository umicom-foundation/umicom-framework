/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_required_rule.c
 *
 * PURPOSE:
 *   Exercise the required rule reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/required_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveRequiredRule item; umi_ui_reactive_required_rule_init(&item); return umi_ui_reactive_required_rule_valid(&item) ? 0 : 1; }
