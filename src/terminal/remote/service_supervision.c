/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/service_supervision.c
 *
 * PURPOSE:
 *   Implement deterministic supervised-service action selection.
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
#include "umicom/terminal/remote/service_supervision.h"
UmiTerminalRemoteServiceSupervisionAction umi_terminal_remote_service_supervision_decide(UmiTerminalRemoteState desired,UmiTerminalRemoteState observed,UmiTerminalRemoteHealth health) { if(desired==UMI_TERMINAL_REMOTE_STATE_STOPPED&&observed!=UMI_TERMINAL_REMOTE_STATE_STOPPED) return UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_STOP; if(desired==UMI_TERMINAL_REMOTE_STATE_ACTIVE&&(observed==UMI_TERMINAL_REMOTE_STATE_STOPPED||observed==UMI_TERMINAL_REMOTE_STATE_READY)) return UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_START; if(desired==UMI_TERMINAL_REMOTE_STATE_ACTIVE&&health==UMI_TERMINAL_REMOTE_HEALTH_CRITICAL) return UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_RESTART; return UMI_TERMINAL_REMOTE_SERVICE_SUPERVISION_NONE; }
