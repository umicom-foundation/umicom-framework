/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_command.c
 *
 * PURPOSE:
 *   Verify remote commands require explicit program and working directory.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_command.h"
int main(void) { UmiTerminalRemoteRemoteCommand v; umi_terminal_remote_remote_command_init(&v,"cmake","/work",false); if(!umi_terminal_remote_remote_command_valid(&v)) return 1; umi_terminal_remote_remote_command_init(&v,"","/work",false); return umi_terminal_remote_remote_command_valid(&v)?2:0; }
