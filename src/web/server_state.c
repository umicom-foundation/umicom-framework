/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_state.c
 *
 * PURPOSE:
 *   Implement observable native-server lifecycle state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/server_state.h"
#include <string.h>
/*
 * Initialise web server state from caller-provided values so later operations receive a
 * known state.
 */
void umi_web_server_state_init(UmiWebServerState *state){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(state==NULL)return;(void)memset(state,0,sizeof(*state));state->phase=UMI_WEB_SERVER_STOPPED;state->last_status=UMI_STATUS_OK;}
/*
 * Provide the web server phase text operation used by this module and its client
 * applications.
 */
const char *umi_web_server_phase_text(UmiWebServerPhase phase){/* Select the behaviour associated with the requested command or state value. */ switch(phase){case UMI_WEB_SERVER_STOPPED:return "stopped";case UMI_WEB_SERVER_STARTING:return "starting";case UMI_WEB_SERVER_READY:return "ready";case UMI_WEB_SERVER_STOPPING:return "stopping";case UMI_WEB_SERVER_FAILED:return "failed";default:return "unknown";}}
