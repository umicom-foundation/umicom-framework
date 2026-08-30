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
void umi_terminal_remote_terminal_output_backpressure_init(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t capacity_bytes,size_t high_watermark_bytes) { if(value) { value->capacity_bytes=capacity_bytes; value->queued_bytes=0U; value->high_watermark_bytes=high_watermark_bytes<=capacity_bytes?high_watermark_bytes:capacity_bytes; } }
UmiStatus umi_terminal_remote_terminal_output_backpressure_enqueue(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes) { if(!value) return UMI_STATUS_INVALID_ARGUMENT; if(bytes>value->capacity_bytes-value->queued_bytes) return UMI_STATUS_CAPACITY_EXCEEDED; value->queued_bytes+=bytes; return UMI_STATUS_OK; }
void umi_terminal_remote_terminal_output_backpressure_consume(UmiTerminalRemoteTerminalOutputBackpressure *value,size_t bytes) { if(value) value->queued_bytes=bytes>=value->queued_bytes?0U:value->queued_bytes-bytes; }
bool umi_terminal_remote_terminal_output_backpressure_throttled(const UmiTerminalRemoteTerminalOutputBackpressure *value) { return value&&value->queued_bytes>=value->high_watermark_bytes; }
