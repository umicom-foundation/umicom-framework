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
typedef struct UmiTerminalRemoteServiceRestart { uint32_t max_attempts; uint32_t attempts; } UmiTerminalRemoteServiceRestart;
void umi_terminal_remote_service_restart_init(UmiTerminalRemoteServiceRestart *value,uint32_t max_attempts);
bool umi_terminal_remote_service_restart_claim_attempt(UmiTerminalRemoteServiceRestart *value);
void umi_terminal_remote_service_restart_reset(UmiTerminalRemoteServiceRestart *value);
#ifdef __cplusplus
}
#endif
#endif
