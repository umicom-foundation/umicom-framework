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
/*
 * Initialise terminal remote remote heartbeat from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_heartbeat_init(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms,uint64_t warning_after_ms,uint64_t critical_after_ms) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value) { value->last_seen_ms=now_ms; value->warning_after_ms=warning_after_ms; value->critical_after_ms=critical_after_ms>=warning_after_ms?critical_after_ms:warning_after_ms; value->revision=1U; } }
/*
 * Provide the terminal remote remote heartbeat observe operation used by this module and
 * its client applications.
 */
void umi_terminal_remote_remote_heartbeat_observe(UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->last_seen_ms=now_ms; value->revision++; } }
/*
 * Provide the terminal remote remote heartbeat health operation used by this module and
 * its client applications.
 */
UmiTerminalRemoteHealth umi_terminal_remote_remote_heartbeat_health(const UmiTerminalRemoteRemoteHeartbeat *value,uint64_t now_ms) { uint64_t age; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||now_ms<value->last_seen_ms) return UMI_TERMINAL_REMOTE_HEALTH_UNKNOWN; age=now_ms-value->last_seen_ms; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(age>=value->critical_after_ms) return UMI_TERMINAL_REMOTE_HEALTH_CRITICAL; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(age>=value->warning_after_ms) return UMI_TERMINAL_REMOTE_HEALTH_WARNING; return UMI_TERMINAL_REMOTE_HEALTH_HEALTHY; }
