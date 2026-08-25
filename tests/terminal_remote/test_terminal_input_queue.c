/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_input_queue.c
 *
 * PURPOSE:
 *   Verify FIFO ordering for terminal input queue.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_input_queue.h"
int main(void) { UmiTerminalRemoteTerminalInputQueue v; uint64_t n=0U; umi_terminal_remote_terminal_input_queue_init(&v); if(umi_terminal_remote_terminal_input_queue_push(&v,7U)!=UMI_STATUS_OK||umi_terminal_remote_terminal_input_queue_push(&v,9U)!=UMI_STATUS_OK) return 1; if(umi_terminal_remote_terminal_input_queue_pop(&v,&n)!=UMI_STATUS_OK||n!=7U) return 2; return 0; }
