/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/service_restart.h
 *
 * PURPOSE:
 *   Bound restart attempts for service restart.
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
#ifndef UMICOM_TERMINAL_REMOTE_SERVICE_RESTART_H
#define UMICOM_TERMINAL_REMOTE_SERVICE_RESTART_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote service restart data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteServiceRestart { uint32_t max_attempts; uint32_t attempts; } UmiTerminalRemoteServiceRestart;
/**
 * Initialise terminal remote service restart from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_service_restart_init(UmiTerminalRemoteServiceRestart *value,uint32_t max_attempts);
/**
 * Provide the terminal remote service restart claim attempt operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_service_restart_claim_attempt(UmiTerminalRemoteServiceRestart *value);
/**
 * Release or reset state held by terminal remote service restart so the same storage can
 * be reused safely.
 */
void umi_terminal_remote_service_restart_reset(UmiTerminalRemoteServiceRestart *value);
#ifdef __cplusplus
}
#endif
#endif
