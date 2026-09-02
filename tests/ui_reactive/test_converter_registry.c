/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_converter_registry.c
 *
 * PURPOSE:
 *   Exercise the converter registry reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter_registry.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveConverterRegistry item; umi_ui_reactive_converter_registry_init(&item); return umi_ui_reactive_converter_registry_valid(&item) ? 0 : 1; }
