/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_snapshot.c
 *
 * PURPOSE:
 *   Exercise the state snapshot reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveStateSnapshot item; umi_ui_reactive_state_snapshot_init(&item); return umi_ui_reactive_state_snapshot_valid(&item) ? 0 : 1; }
