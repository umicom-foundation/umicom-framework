/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/terminal_remote/test_remote_connection_policy.c
 *
 * PURPOSE:
 *   Verify remote connection policy defaults to trusted, read-only operation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_connection_policy.h"
int main(void) { UmiTerminalRemoteRemoteConnectionPolicy v; umi_terminal_remote_remote_connection_policy_init(&v); if(umi_terminal_remote_remote_connection_policy_allows(&v,false,false,false)) return 1; if(umi_terminal_remote_remote_connection_policy_allows(&v,true,true,false)) return 2; return umi_terminal_remote_remote_connection_policy_allows(&v,true,false,true)?0:3; }
