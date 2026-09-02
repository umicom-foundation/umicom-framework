/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_descriptor.c
 *
 * PURPOSE:
 *   Exercise the binding descriptor reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveBindingDescriptor item; umi_ui_reactive_binding_descriptor_init(&item); return umi_ui_reactive_binding_descriptor_valid(&item) ? 0 : 1; }
