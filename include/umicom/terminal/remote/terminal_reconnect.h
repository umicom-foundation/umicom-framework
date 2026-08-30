/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_reconnect.h
 *
 * PURPOSE:
 *   Compute bounded exponential retry delays for terminal reconnect.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_RECONNECT_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_RECONNECT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalReconnect { uint64_t initial_ms; uint64_t maximum_ms; uint32_t attempts; } UmiTerminalRemoteTerminalReconnect;
void umi_terminal_remote_terminal_reconnect_init(UmiTerminalRemoteTerminalReconnect *value,uint64_t initial_ms,uint64_t maximum_ms);
uint64_t umi_terminal_remote_terminal_reconnect_next_delay(UmiTerminalRemoteTerminalReconnect *value);
void umi_terminal_remote_terminal_reconnect_reset(UmiTerminalRemoteTerminalReconnect *value);
#ifdef __cplusplus
}
#endif
#endif
