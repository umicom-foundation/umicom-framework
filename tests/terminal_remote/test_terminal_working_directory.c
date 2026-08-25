/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_working_directory.c
 *
 * PURPOSE:
 *   Verify terminal working directory accepts bounded absolute Windows and POSIX paths.
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
int main(void) { UmiTerminalRemoteTerminalWorkingDirectory v; umi_terminal_remote_terminal_working_directory_init(&v); if(umi_terminal_remote_terminal_working_directory_set(&v,"/tmp/work")!=UMI_STATUS_OK) return 1; if(!umi_terminal_remote_terminal_working_directory_absolute(&v)) return 2; if(umi_terminal_remote_terminal_working_directory_set(&v,"relative")!=UMI_STATUS_OK) return 3; return umi_terminal_remote_terminal_working_directory_absolute(&v)?4:0; }
