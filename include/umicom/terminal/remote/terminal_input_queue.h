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
/**
 * Represent the terminal remote terminal input queue data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalInputQueue { uint64_t items[UMI_TERMINAL_REMOTE_TERMINAL_INPUT_QUEUE_CAPACITY]; size_t head; size_t count; } UmiTerminalRemoteTerminalInputQueue;
/**
 * Initialise terminal remote terminal input queue from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_input_queue_init(UmiTerminalRemoteTerminalInputQueue *value);
/**
 * Provide the terminal remote terminal input queue push operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_terminal_input_queue_push(UmiTerminalRemoteTerminalInputQueue *value,uint64_t item);
/**
 * Provide the terminal remote terminal input queue pop operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_terminal_input_queue_pop(UmiTerminalRemoteTerminalInputQueue *value,uint64_t *out_item);
#ifdef __cplusplus
}
#endif
#endif
