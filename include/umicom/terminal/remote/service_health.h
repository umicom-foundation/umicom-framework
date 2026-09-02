/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/service_health.h
 *
 * PURPOSE:
 *   Aggregate failure and latency evidence into service health health.
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
#ifndef UMICOM_TERMINAL_REMOTE_SERVICE_HEALTH_H
#define UMICOM_TERMINAL_REMOTE_SERVICE_HEALTH_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the terminal remote service health evaluate operation used by this module and
 * its client applications.
 */
UmiTerminalRemoteHealth umi_terminal_remote_service_health_evaluate(uint32_t consecutive_failures,uint64_t latency_ms,uint64_t warning_latency_ms);
#ifdef __cplusplus
}
#endif
#endif
