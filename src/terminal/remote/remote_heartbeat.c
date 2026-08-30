/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_heartbeat.c
 *
 * PURPOSE:
 *   Implement deterministic heartbeat health for remote heartbeat.
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
#include "umicom/terminal/remote/remote_heartbeat.h"
void umi_terminal_remote_remote_heartbeat_init(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms,uint64_t warning_after_ms,uint64_t critical_after_ms) { if(value) { value->last_seen_ms=now_ms; value->warning_after_ms=warning_after_ms; value->critical_after_ms=critical_after_ms>=warning_after_ms?critical_after_ms:warning_after_ms; value->revision=1U; } }
void umi_terminal_remote_remote_heartbeat_observe(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms) { if(value) { value->last_seen_ms=now_ms; value->revision++; } }
UmiTerminalRemoteHealth umi_terminal_remote_remote_heartbeat_health(const UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms) { uint64_t age; if(!value||now_ms<value->last_seen_ms) return UMI_TERMINAL_REMOTE_HEALTH_UNKNOWN; age=now_ms-value->last_seen_ms; if(age>=value->critical_after_ms) return UMI_TERMINAL_REMOTE_HEALTH_CRITICAL; if(age>=value->warning_after_ms) return UMI_TERMINAL_REMOTE_HEALTH_WARNING; return UMI_TERMINAL_REMOTE_HEALTH_HEALTHY; }
