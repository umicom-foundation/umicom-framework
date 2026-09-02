/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_converter.c
 *
 * PURPOSE:
 *   Exercise the converter reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveConverter item; umi_ui_reactive_converter_init(&item); return umi_ui_reactive_converter_valid(&item) ? 0 : 1; }
