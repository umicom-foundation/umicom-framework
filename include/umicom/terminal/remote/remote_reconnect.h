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
/**
 * Represent the terminal remote remote reconnect data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteReconnect { uint64_t initial_ms; uint64_t maximum_ms; uint32_t attempts; } UmiTerminalRemoteRemoteReconnect;
/**
 * Initialise terminal remote remote reconnect from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_reconnect_init(UmiTerminalRemoteRemoteReconnect *value,uint64_t initial_ms,uint64_t maximum_ms);
/**
 * Provide the terminal remote remote reconnect next delay operation used by this module
 * and its client applications.
 */
uint64_t umi_terminal_remote_remote_reconnect_next_delay(UmiTerminalRemoteRemoteReconnect *value);
/**
 * Release or reset state held by terminal remote remote reconnect so the same storage can
 * be reused safely.
 */
void umi_terminal_remote_remote_reconnect_reset(UmiTerminalRemoteRemoteReconnect *value);
#ifdef __cplusplus
}
#endif
#endif
