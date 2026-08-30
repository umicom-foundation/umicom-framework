/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_input_queue.c
 *
 * PURPOSE:
 *   Implement bounded FIFO operations for terminal input queue.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_input_queue.h"
#include <string.h>
void umi_terminal_remote_terminal_input_queue_init(UmiTerminalRemoteTerminalInputQueue *value) { if(value) (void)memset(value,0,sizeof(*value)); }
UmiStatus umi_terminal_remote_terminal_input_queue_push(UmiTerminalRemoteTerminalInputQueue *value,uint64_t item) { size_t index; if(!value) return UMI_STATUS_INVALID_ARGUMENT; if(value->count>=UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; index=(value->head+value->count)%UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY; value->items[index]=item; value->count++; return UMI_STATUS_OK; }
UmiStatus umi_terminal_remote_terminal_input_queue_pop(UmiTerminalRemoteTerminalInputQueue *value,uint64_t *out_item) { if(!value||!out_item) return UMI_STATUS_INVALID_ARGUMENT; if(value->count==0U) return UMI_STATUS_NOT_FOUND; *out_item=value->items[value->head]; value->head=(value->head+1U)%UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY; value->count--; return UMI_STATUS_OK; }
