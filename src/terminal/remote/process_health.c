/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_health.c
 *
 * PURPOSE:
 *   Implement severity aggregation for process health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/process_health.h"
UmiTerminalRemoteHealth umi_terminal_remote_process_health_evaluate(uint32_t consecutive_failures,uint64_t latency_ms,uint64_t warning_latency_ms) { if(consecutive_failures>=3U) return UMI_TERMINAL_REMOTE_HEALTH_CRITICAL; if(consecutive_failures>0U || (warning_latency_ms>0U && latency_ms>=warning_latency_ms)) return UMI_TERMINAL_REMOTE_HEALTH_WARNING; return UMI_TERMINAL_REMOTE_HEALTH_HEALTHY; }
