/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_heartbeat.h
 *
 * PURPOSE:
 *   Track liveness timestamps and health for terminal heartbeat.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_HEARTBEAT_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_HEARTBEAT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalHeartbeat { uint64_t last_seen_ms; uint64_t warning_after_ms; uint64_t critical_after_ms; uint64_t revision; } UmiTerminalRemoteTerminalHeartbeat;
void umi_terminal_remote_terminal_heartbeat_init(UmiTerminalRemoteTerminalHeartbeat *value,uint64_t now_ms,uint64_t warning_after_ms,uint64_t critical_after_ms);
void umi_terminal_remote_terminal_heartbeat_observe(UmiTerminalRemoteTerminalHeartbeat *value,uint64_t now_ms);
UmiTerminalRemoteHealth umi_terminal_remote_terminal_heartbeat_health(const UmiTerminalRemoteTerminalHeartbeat *value,uint64_t now_ms);
#ifdef __cplusplus
}
#endif
#endif
