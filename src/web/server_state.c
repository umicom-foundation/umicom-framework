/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/server_state.c
 *
 * PURPOSE:
 *   Implement observable native-server lifecycle state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/server_state.h"
#include <string.h>
void umi_web_server_state_init(UmiWebServerState *state){if(state==NULL)return;(void)memset(state,0,sizeof(*state));state->phase=UMI_WEB_SERVER_STOPPED;state->last_status=UMI_STATUS_OK;}
const char *umi_web_server_phase_text(UmiWebServerPhase phase){switch(phase){case UMI_WEB_SERVER_STOPPED:return "stopped";case UMI_WEB_SERVER_STARTING:return "starting";case UMI_WEB_SERVER_READY:return "ready";case UMI_WEB_SERVER_STOPPING:return "stopping";case UMI_WEB_SERVER_FAILED:return "failed";default:return "unknown";}}
