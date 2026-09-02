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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveChangeTransaction t;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_reactive_change_transaction_begin(&t,"edit-1",4U)!=UMI_STATUS_OK)return 1;t.change_count=2U;return umi_ui_reactive_change_transaction_commit(&t)==UMI_STATUS_OK?0:2; }
