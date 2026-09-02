/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/background_service.h
 *
 * PURPOSE:
 *   Describe a supervised reusable background service and desired state.
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
#ifndef UMICOM_TERMINAL_REMOTE_BACKGROUND_SERVICE_H
#define UMICOM_TERMINAL_REMOTE_BACKGROUND_SERVICE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote background service data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteBackgroundService { UmiTerminalRemoteNamedEntry identity; UmiTerminalRemoteState desired_state; UmiTerminalRemoteState observed_state; } UmiTerminalRemoteBackgroundService;
/**
 * Initialise terminal remote background service from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_background_service_init(UmiTerminalRemoteBackgroundService *value,const char *id);
/**
 * Provide the terminal remote background service converged operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_background_service_converged(const UmiTerminalRemoteBackgroundService *value);
#ifdef __cplusplus
}
#endif
#endif
