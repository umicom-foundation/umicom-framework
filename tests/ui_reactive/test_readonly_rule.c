/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_readonly_rule.c
 *
 * PURPOSE:
 *   Exercise the readonly rule reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/readonly_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveReadonlyRule item; umi_ui_reactive_readonly_rule_init(&item); return umi_ui_reactive_readonly_rule_valid(&item) ? 0 : 1; }
