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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_working_directory.h"
#include <string.h>
void umi_terminal_remote_terminal_working_directory_init(UmiTerminalRemoteTerminalWorkingDirectory *value) { if(value) { (void)memset(value,0,sizeof(*value)); value->revision=1U; } }
UmiStatus umi_terminal_remote_terminal_working_directory_set(UmiTerminalRemoteTerminalWorkingDirectory *value,const char *path) { UmiStatus s; if(!value||!path||path[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; s=umi_terminal_remote_copy_text(value->path,sizeof(value->path),path); if(s==UMI_STATUS_OK) value->revision++; return s; }
bool umi_terminal_remote_terminal_working_directory_absolute(const UmiTerminalRemoteTerminalWorkingDirectory *value) { const char *p; if(!value) return false; p=value->path; return p[0]=='/' || (p[0]!='\0'&&p[1]==':'); }
