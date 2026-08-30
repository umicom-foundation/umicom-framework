/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_input_queue.h
 *
 * PURPOSE:
 *   Maintain a bounded FIFO unit queue for terminal input queue.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY 16U
typedef struct UmiTerminalRemoteTerminalInputQueue { uint64_t items[UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY]; size_t head; size_t count; } UmiTerminalRemoteTerminalInputQueue;
void umi_terminal_remote_terminal_input_queue_init(UmiTerminalRemoteTerminalInputQueue *value);
UmiStatus umi_terminal_remote_terminal_input_queue_push(UmiTerminalRemoteTerminalInputQueue *value,uint64_t item);
UmiStatus umi_terminal_remote_terminal_input_queue_pop(UmiTerminalRemoteTerminalInputQueue *value,uint64_t *out_item);
#ifdef __cplusplus
}
#endif
#endif
