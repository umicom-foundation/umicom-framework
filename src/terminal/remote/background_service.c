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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/background_service.h"
/*
 * Initialise terminal remote background service from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_background_service_init(UmiTerminalRemoteBackgroundService *value,const char *id) { /* Apply this operation only while the related capability or state is available. */ if(value) { umi_terminal_remote_named_entry_init(&value->identity,id,"background service"); value->desired_state=UMI_TERMINAL_REMOTE_STATE_ACTIVE; value->observed_state=UMI_TERMINAL_REMOTE_STATE_READY; } }
/*
 * Provide the terminal remote background service converged operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_background_service_converged(const UmiTerminalRemoteBackgroundService *value) { return value&&umi_terminal_remote_named_entry_valid(&value->identity)&&value->desired_state==value->observed_state; }
