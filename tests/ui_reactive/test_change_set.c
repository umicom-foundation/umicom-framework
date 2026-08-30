/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_change_set.c
 *
 * PURPOSE:
 *   Exercise the change set reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_set.h"
int main(void) { UmiUiReactiveChangeSet item; umi_ui_reactive_change_set_init(&item); return umi_ui_reactive_change_set_valid(&item) ? 0 : 1; }
