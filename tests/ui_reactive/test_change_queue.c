/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_change_queue.c
 *
 * PURPOSE:
 *   Exercise the change queue reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_queue.h"
int main(void) { UmiUiReactiveChangeQueue q; uint64_t v=0; umi_ui_reactive_change_queue_init(&q); if(umi_ui_reactive_change_queue_push(&q,7U)!=UMI_STATUS_OK) return 1; if(umi_ui_reactive_change_queue_pop(&q,&v)!=UMI_STATUS_OK) return 2; return v==7U?0:3; }
