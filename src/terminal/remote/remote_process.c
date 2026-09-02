/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_process.c
 *
 * PURPOSE:
 *   Implement guarded remote process lifecycle transitions.
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
#include "umicom/terminal/remote/remote_process.h"
#include <string.h>
/*
 * Initialise terminal remote remote process from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_process_init(UmiTerminalRemoteRemoteProcess *value,const char *id) { /* Apply this operation only while the related capability or state is available. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this operation only while the related capability or state is available. */ if(id) (void)umi_terminal_remote_copy_text(value->id,sizeof(value->id),id); value->state=UMI_TERMINAL_REMOTE_STATE_READY; value->revision=1U; }
/*
 * Provide the terminal remote remote process transition operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_remote_process_transition(UmiTerminalRemoteRemoteProcess *value,UmiTerminalRemoteState next) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value||value->id[0]=='\0'||next==UMI_TERMINAL_REMOTE_STATE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(value->state==UMI_TERMINAL_REMOTE_STATE_FAILED && next==UMI_TERMINAL_REMOTE_STATE_ACTIVE) return UMI_STATUS_INVALID_STATE; value->state=next; value->revision++; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote remote process usable operation used by this module and its
 * client applications.
 */
bool umi_terminal_remote_remote_process_usable(const UmiTerminalRemoteRemoteProcess *value) { return value && (value->state==UMI_TERMINAL_REMOTE_STATE_READY||value->state==UMI_TERMINAL_REMOTE_STATE_ACTIVE||value->state==UMI_TERMINAL_REMOTE_STATE_DEGRADED); }
