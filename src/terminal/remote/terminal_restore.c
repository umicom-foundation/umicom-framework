/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_restore.c
 *
 * PURPOSE:
 *   Implement guarded terminal restore lifecycle transitions.
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
#include "umicom/terminal/remote/terminal_restore.h"
#include <string.h>
void umi_terminal_remote_terminal_restore_init(UmiTerminalRemoteTerminalRestore *value,const char *id) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(id) (void)umi_terminal_remote_copy_text(value->id,sizeof(value->id),id); value->state=UMI_TERMINAL_REMOTE_STATE_READY; value->revision=1U; }
UmiStatus umi_terminal_remote_terminal_restore_transition(UmiTerminalRemoteTerminalRestore *value,UmiTerminalRemoteState next) { if(!value||value->id[0]=='\0'||next==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; if(value->state==UMI_TERMINAL_REMOTE_STATE_FAILED && next==UMI_TERMINAL_REMOTE_STATE_ACTIVE) return UMI_STATUS_INVALID_STATE; value->state=next; value->revision++; return UMI_STATUS_OK; }
bool umi_terminal_remote_terminal_restore_usable(const UmiTerminalRemoteTerminalRestore *value) { return value && (value->state==UMI_TERMINAL_REMOTE_STATE_READY||value->state==UMI_TERMINAL_REMOTE_STATE_ACTIVE||value->state==UMI_TERMINAL_REMOTE_STATE_DEGRADED); }
