/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_batch_update.c
 *
 * PURPOSE:
 *   Exercise the batch update reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/batch_update.h"
int main(void) { UmiUiReactiveBatchUpdate item; umi_ui_reactive_batch_update_init(&item); return umi_ui_reactive_batch_update_valid(&item) ? 0 : 1; }
