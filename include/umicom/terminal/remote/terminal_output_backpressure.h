/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_output_backpressure.h
 *
 * PURPOSE:
 *   Bound terminal output queues and signal producer throttling before memory pressure.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_BACKPRESSURE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_BACKPRESSURE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal output backpressure data shared with callers of
 * this public contract.
 */
typedef struct UmiTerminalRemoteTerminalOutputBackpressure { size_t capacity_bytes; size_t queued_bytes; size_t high_watermark_bytes; } UmiTerminalRemoteTerminalOutputBackpressure;
/**
 * Initialise terminal remote terminal output backpressure from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_output_backpressure_init(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t capacity_bytes,size_t high_watermark_bytes);
/**
 * Provide the terminal remote terminal output backpressure enqueue operation used by this
 * module and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_output_backpressure_enqueue(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes);
/**
 * Provide the terminal remote terminal output backpressure consume operation used by this
 * module and its client applications.
 */
void umi_terminal_remote_terminal_output_backpressure_consume(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes);
/**
 * Provide the terminal remote terminal output backpressure throttled operation used by
 * this module and its client applications.
 */
bool umi_terminal_remote_terminal_output_backpressure_throttled(const UmiTerminalRemoteTerminalOutputBackpressure *value);
#ifdef __cplusplus
}
#endif
#endif
