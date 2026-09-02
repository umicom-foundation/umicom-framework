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
/*
 * Initialise terminal remote terminal restore from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_restore_init(UmiTerminalRemoteTerminalRestore *value,const char *id) { /* Apply this operation only while the related capability or state is available. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this operation only while the related capability or state is available. */ if(id) (void)umi_terminal_remote_copy_text(value->id,sizeof(value->id),id); value->state=UMI_TERMINAL_REMOTE_STATE_READY; value->revision=1U; }
/*
 * Provide the terminal remote terminal restore transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_restore_transition(UmiTerminalRemoteTerminalRestore *value,UmiTerminalRemoteState next) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value||value->id[0]=='\0'||next==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(value->state==UMI_TERMINAL_REMOTE_STATE_FAILED && next==UMI_TERMINAL_REMOTE_STATE_ACTIVE) return UMI_STATUS_INVALID_STATE; value->state=next; value->revision++; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote terminal restore usable operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_terminal_restore_usable(const UmiTerminalRemoteTerminalRestore *value) { return value && (value->state==UMI_TERMINAL_REMOTE_STATE_READY||value->state==UMI_TERMINAL_REMOTE_STATE_ACTIVE||value->state==UMI_TERMINAL_REMOTE_STATE_DEGRADED); }
