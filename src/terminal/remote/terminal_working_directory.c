/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_working_directory.c
 *
 * PURPOSE:
 *   Implement bounded path state for terminal working directory.
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
#include "umicom/terminal/remote/terminal_working_directory.h"
#include <string.h>
/*
 * Initialise terminal remote terminal working directory from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_working_directory_init(UmiTerminalRemoteTerminalWorkingDirectory *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { (void)memset(value,0,sizeof(*value)); value->revision=1U; } }
/*
 * Copy terminal remote terminal working directory into module-owned storage so callers
 * keep ownership of their input values.
 */
UmiStatus umi_terminal_remote_terminal_working_directory_set(UmiTerminalRemoteTerminalWorkingDirectory *value,const char *path) { UmiStatus s; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value||!path||path[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; s=umi_terminal_remote_copy_text(value->path,sizeof(value->path),path); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(s==UMI_STATUS_OK) value->revision++; return s; }
/*
 * Provide the terminal remote terminal working directory absolute operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_terminal_working_directory_absolute(const UmiTerminalRemoteTerminalWorkingDirectory *value) { const char *p; /* Apply this branch only when its contract condition is satisfied. */ if(!value) return false; p=value->path; return p[0]=='/' || (p[0]!='\0'&&p[1]==':'); }
