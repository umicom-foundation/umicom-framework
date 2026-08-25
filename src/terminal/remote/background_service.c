/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/background_service.c
 *
 * PURPOSE:
 *   Implement background-service desired/observed state convergence.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/background_service.h"
void umi_terminal_remote_background_service_init(UmiTerminalRemoteBackgroundService *value,const char *id) { if(value) { umi_terminal_remote_named_entry_init(&value->identity,id,"background service"); value->desired_state=UMI_TERMINAL_REMOTE_STATE_ACTIVE; value->observed_state=UMI_TERMINAL_REMOTE_STATE_READY; } }
bool umi_terminal_remote_background_service_converged(const UmiTerminalRemoteBackgroundService *value) { return value&&umi_terminal_remote_named_entry_valid(&value->identity)&&value->desired_state==value->observed_state; }
