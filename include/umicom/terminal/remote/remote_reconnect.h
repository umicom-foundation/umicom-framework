/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_reconnect.h
 *
 * PURPOSE:
 *   Compute bounded exponential retry delays for remote reconnect.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_RECONNECT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_RECONNECT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteReconnect { uint64_t initial_ms; uint64_t maximum_ms; uint32_t attempts; } UmiTerminalRemoteRemoteReconnect;
void umi_terminal_remote_remote_reconnect_init(UmiTerminalRemoteRemoteReconnect *value,uint64_t initial_ms,uint64_t maximum_ms);
uint64_t umi_terminal_remote_remote_reconnect_next_delay(UmiTerminalRemoteRemoteReconnect *value);
void umi_terminal_remote_remote_reconnect_reset(UmiTerminalRemoteRemoteReconnect *value);
#ifdef __cplusplus
}
#endif
#endif
