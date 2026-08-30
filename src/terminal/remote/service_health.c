/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/service_health.c
 *
 * PURPOSE:
 *   Implement severity aggregation for service health.
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
#include "umicom/terminal/remote/service_health.h"
UmiTerminalRemoteHealth umi_terminal_remote_service_health_evaluate(uint32_t consecutive_failures,uint64_t latency_ms,uint64_t warning_latency_ms) { if(consecutive_failures>=3U) return UMI_TERMINAL_REMOTE_HEALTH_CRITICAL; if(consecutive_failures>0U || (warning_latency_ms>0U && latency_ms>=warning_latency_ms)) return UMI_TERMINAL_REMOTE_HEALTH_WARNING; return UMI_TERMINAL_REMOTE_HEALTH_HEALTHY; }
