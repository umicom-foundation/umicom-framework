/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_endpoint.c
 *
 * PURPOSE:
 *   Exercise the binding endpoint reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_endpoint.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveBindingEndpoint item; umi_ui_reactive_binding_endpoint_init(&item); return umi_ui_reactive_binding_endpoint_valid(&item) ? 0 : 1; }
