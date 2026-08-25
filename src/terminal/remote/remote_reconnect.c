/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_reconnect.c
 *
 * PURPOSE:
 *   Implement overflow-safe bounded exponential backoff for remote reconnect.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_reconnect.h"
void umi_terminal_remote_remote_reconnect_init(UmiTerminalRemoteRemoteReconnect *value,uint64_t initial_ms,uint64_t maximum_ms) { if(value) { value->initial_ms=initial_ms; value->maximum_ms=maximum_ms>=initial_ms?maximum_ms:initial_ms; value->attempts=0U; } }
uint64_t umi_terminal_remote_remote_reconnect_next_delay(UmiTerminalRemoteRemoteReconnect *value) { uint64_t delay; uint32_t i; if(!value||value->initial_ms==0U) return 0U; delay=value->initial_ms; for(i=0U;i<value->attempts && delay<value->maximum_ms;++i) { if(delay>value->maximum_ms/2U) { delay=value->maximum_ms; break; } delay*=2U; } if(delay>value->maximum_ms) delay=value->maximum_ms; if(value->attempts<UINT32_MAX) value->attempts++; return delay; }
void umi_terminal_remote_remote_reconnect_reset(UmiTerminalRemoteRemoteReconnect *value) { if(value) value->attempts=0U; }
