/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_change_transaction.c
 *
 * PURPOSE:
 *   Exercise the change transaction reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_transaction.h"
int main(void) { UmiUiReactiveChangeTransaction t;if(umi_ui_reactive_change_transaction_begin(&t,"edit-1",4U)!=UMI_STATUS_OK)return 1;t.change_count=2U;return umi_ui_reactive_change_transaction_commit(&t)==UMI_STATUS_OK?0:2; }
