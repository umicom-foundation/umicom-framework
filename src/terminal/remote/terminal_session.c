/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_session.c
 *
 * PURPOSE:
 *   Implement guarded terminal session lifecycle transitions.
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
#include "umicom/terminal/remote/terminal_session.h"
#include <string.h>
void umi_terminal_remote_terminal_session_init(UmiTerminalRemoteTerminalSession *value,const char *id) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(id) (void)umi_terminal_remote_copy_text(value->id,sizeof(value->id),id); value->state=UMI_TERMINAL_REMOTE_STATE_READY; value->revision=1U; }
UmiStatus umi_terminal_remote_terminal_session_transition(UmiTerminalRemoteTerminalSession *value,UmiTerminalRemoteState next) { if(!value||value->id[0]=='\0'||next==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; if(value->state==UMI_TERMINAL_REMOTE_STATE_FAILED && next==UMI_TERMINAL_REMOTE_STATE_ACTIVE) return UMI_STATUS_INVALID_STATE; value->state=next; value->revision++; return UMI_STATUS_OK; }
bool umi_terminal_remote_terminal_session_usable(const UmiTerminalRemoteTerminalSession *value) { return value && (value->state==UMI_TERMINAL_REMOTE_STATE_READY||value->state==UMI_TERMINAL_REMOTE_STATE_ACTIVE||value->state==UMI_TERMINAL_REMOTE_STATE_DEGRADED); }
