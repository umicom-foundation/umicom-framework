/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_health.h
 *
 * PURPOSE:
 *   Aggregate failure and latency evidence into terminal health health.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_HEALTH_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_HEALTH_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiTerminalRemoteHealth umi_terminal_remote_terminal_health_evaluate(uint32_t consecutive_failures,uint64_t latency_ms,uint64_t warning_latency_ms);
#ifdef __cplusplus
}
#endif
#endif
