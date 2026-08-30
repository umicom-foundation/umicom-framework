/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_session.c
 *
 * PURPOSE:
 *   Exercise the binding session reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_session.h"
int main(void) { UmiUiReactiveBindingSession item; umi_ui_reactive_binding_session_init(&item); return umi_ui_reactive_binding_session_valid(&item) ? 0 : 1; }
