/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_restore.c
 *
 * PURPOSE:
 *   Exercise the state restore reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_restore.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveStateRestore item; umi_ui_reactive_state_restore_init(&item); return umi_ui_reactive_state_restore_valid(&item) ? 0 : 1; }
