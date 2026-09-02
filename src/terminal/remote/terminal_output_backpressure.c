/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_output_backpressure.c
 *
 * PURPOSE:
 *   Implement bounded output backpressure accounting.
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
#include "umicom/terminal/remote/terminal_output_backpressure.h"
/*
 * Initialise terminal remote terminal output backpressure from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_output_backpressure_init(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t capacity_bytes,size_t high_watermark_bytes) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value) { value->capacity_bytes=capacity_bytes; value->queued_bytes=0U; value->high_watermark_bytes=high_watermark_bytes<=capacity_bytes?high_watermark_bytes:capacity_bytes; } }
/*
 * Provide the terminal remote terminal output backpressure enqueue operation used by this
 * module and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_output_backpressure_enqueue(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value) return UMI_STATUS_INVALID_ARGUMENT; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(bytes>value->capacity_bytes-value->queued_bytes) return UMI_STATUS_CAPACITY_EXCEEDED; value->queued_bytes+=bytes; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote terminal output backpressure consume operation used by this
 * module and its client applications.
 */
void umi_terminal_remote_terminal_output_backpressure_consume(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value) value->queued_bytes=bytes>=value->queued_bytes?0U:value->queued_bytes-bytes; }
/*
 * Provide the terminal remote terminal output backpressure throttled operation used by
 * this module and its client applications.
 */
bool umi_terminal_remote_terminal_output_backpressure_throttled(const UmiTerminalRemoteTerminalOutputBackpressure *value) { return value&&value->queued_bytes>=value->high_watermark_bytes; }
