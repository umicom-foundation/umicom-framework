/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_visibility_rule.c
 *
 * PURPOSE:
 *   Exercise the visibility rule reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/visibility_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveVisibilityRule item; umi_ui_reactive_visibility_rule_init(&item); return umi_ui_reactive_visibility_rule_valid(&item) ? 0 : 1; }
