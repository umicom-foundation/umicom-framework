/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_snapshot.c
 *
 * PURPOSE:
 *   Exercise the state snapshot reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_snapshot.h"
int main(void) { UmiUiReactiveStateSnapshot item; umi_ui_reactive_state_snapshot_init(&item); return umi_ui_reactive_state_snapshot_valid(&item) ? 0 : 1; }
