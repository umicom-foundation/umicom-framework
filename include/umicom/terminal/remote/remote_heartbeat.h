/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_heartbeat.h
 *
 * PURPOSE:
 *   Track liveness timestamps and health for remote heartbeat.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_HEARTBEAT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_HEARTBEAT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote heartbeat data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteHeartbeat { uint64_t last_seen_ms; uint64_t warning_after_ms; uint64_t critical_after_ms; uint64_t revision; } UmiTerminalRemoteRemoteHeartbeat;
/**
 * Initialise terminal remote remote heartbeat from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_heartbeat_init(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms,uint64_t warning_after_ms,uint64_t critical_after_ms);
/**
 * Provide the terminal remote remote heartbeat observe operation used by this module and
 * its client applications.
 */
void umi_terminal_remote_remote_heartbeat_observe(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms);
/**
 * Provide the terminal remote remote heartbeat health operation used by this module and
 * its client applications.
 */
UmiTerminalRemoteHealth umi_terminal_remote_remote_heartbeat_health(const UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
