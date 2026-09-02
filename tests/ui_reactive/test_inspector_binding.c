/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_inspector_binding.c
 *
 * PURPOSE:
 *   Exercise the inspector binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/inspector_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveInspectorBinding item; umi_ui_reactive_inspector_binding_init(&item); return umi_ui_reactive_inspector_binding_valid(&item) ? 0 : 1; }
