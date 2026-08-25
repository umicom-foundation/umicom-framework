/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_terminal_command_policy.c
 *
 * PURPOSE:
 *   Verify terminal command policy defaults to trusted, read-only operation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_command_policy.h"
int main(void) { UmiTerminalRemoteTerminalCommandPolicy v; umi_terminal_remote_terminal_command_policy_init(&v); if(umi_terminal_remote_terminal_command_policy_allows(&v,false,false,false)) return 1; if(umi_terminal_remote_terminal_command_policy_allows(&v,true,true,false)) return 2; return umi_terminal_remote_terminal_command_policy_allows(&v,true,false,true)?0:3; }
