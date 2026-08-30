/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_propagation_guard.c
 *
 * PURPOSE:
 *   Exercise the propagation guard reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_guard.h"
int main(void) { UmiUiReactivePropagationGuard g={0U,2U,false};if(umi_ui_reactive_propagation_guard_enter(&g)!=UMI_STATUS_OK)return 1;umi_ui_reactive_propagation_guard_leave(&g);return g.depth==0U?0:2; }
