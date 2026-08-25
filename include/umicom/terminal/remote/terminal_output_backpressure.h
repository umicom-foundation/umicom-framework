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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_BACKPRESSURE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_OUTPUT_BACKPRESSURE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalOutputBackpressure { size_t capacity_bytes; size_t queued_bytes; size_t high_watermark_bytes; } UmiTerminalRemoteTerminalOutputBackpressure;
void umi_terminal_remote_terminal_output_backpressure_init(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t capacity_bytes,size_t high_watermark_bytes);
UmiStatus umi_terminal_remote_terminal_output_backpressure_enqueue(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes);
void umi_terminal_remote_terminal_output_backpressure_consume(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes);
bool umi_terminal_remote_terminal_output_backpressure_throttled(const UmiTerminalRemoteTerminalOutputBackpressure *value);
#ifdef __cplusplus
}
#endif
#endif
