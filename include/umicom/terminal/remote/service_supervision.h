/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/service_supervision.h
 *
 * PURPOSE:
 *   Choose supervised service action from desired/observed state and health.
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
#ifndef UMICOM_TERMINAL_REMOTE_SERVICE_SUPERVISION_H
#define UMICOM_TERMINAL_REMOTE_SERVICE_SUPERVISION_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiTerminalRemoteServiceSupervisionAction { UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_NONE=0, UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_START=1, UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_RESTART=2, UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_STOP=3 } UmiTerminalRemoteServiceSupervisionAction;
UmiTerminalRemoteServiceSupervisionAction umi_terminal_remote_service_supervision_decide(UmiTerminalRemoteState desired,UmiTerminalRemoteState observed,UmiTerminalRemoteHealth health);
#ifdef __cplusplus
}
#endif
#endif
