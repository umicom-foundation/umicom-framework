/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_state_restore.c
 *
 * PURPOSE:
 *   Exercise the state restore reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_restore.h"
int main(void) { UmiUiReactiveStateRestore item; umi_ui_reactive_state_restore_init(&item); return umi_ui_reactive_state_restore_valid(&item) ? 0 : 1; }
