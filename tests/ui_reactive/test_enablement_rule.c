/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_enablement_rule.c
 *
 * PURPOSE:
 *   Exercise the enablement rule reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/enablement_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveEnablementRule item; umi_ui_reactive_enablement_rule_init(&item); return umi_ui_reactive_enablement_rule_valid(&item) ? 0 : 1; }
