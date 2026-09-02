/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_reconnect.c
 *
 * PURPOSE:
 *   Implement overflow-safe bounded exponential backoff for terminal reconnect.
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
#include "umicom/terminal/remote/terminal_reconnect.h"
/*
 * Initialise terminal remote terminal reconnect from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_reconnect_init(UmiTerminalRemoteTerminalReconnect *value,uint64_t initial_ms,uint64_t maximum_ms) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value) { value->initial_ms=initial_ms; value->maximum_ms=maximum_ms>=initial_ms?maximum_ms:initial_ms; value->attempts=0U; } }
/*
 * Provide the terminal remote terminal reconnect next delay operation used by this module
 * and its client applications.
 */
uint64_t umi_terminal_remote_terminal_reconnect_next_delay(UmiTerminalRemoteTerminalReconnect *value) { uint64_t delay; uint32_t i; /* Apply this branch only when its contract condition is satisfied. */ if(!value||value->initial_ms==0U) return 0U; delay=value->initial_ms; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<value->attempts && delay<value->maximum_ms;++i) { /* Apply this branch only when its contract condition is satisfied. */ if(delay>value->maximum_ms/2U) { delay=value->maximum_ms; break; } delay*=2U; } /* Apply this branch only when its contract condition is satisfied. */ if(delay>value->maximum_ms) delay=value->maximum_ms; /* Apply this branch only when its contract condition is satisfied. */ if(value->attempts<UINT32_MAX) value->attempts++; return delay; }
/*
 * Release or reset state held by terminal remote terminal reconnect so the same storage
 * can be reused safely.
 */
void umi_terminal_remote_terminal_reconnect_reset(UmiTerminalRemoteTerminalReconnect *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) value->attempts=0U; }
