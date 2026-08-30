/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_command_policy.c
 *
 * PURPOSE:
 *   Implement explicit least-privilege gates for terminal command policy.
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
#include "umicom/terminal/remote/terminal_command_policy.h"
void umi_terminal_remote_terminal_command_policy_init(UmiTerminalRemoteTerminalCommandPolicy *value) { if(value) { value->require_trust=true; value->allow_write=false; value->allow_interactive=true; } }
bool umi_terminal_remote_terminal_command_policy_allows(const UmiTerminalRemoteTerminalCommandPolicy *value,bool trusted,bool write_request,bool interactive_request) { if(!value) return false; if(value->require_trust&&!trusted) return false; if(write_request&&!value->allow_write) return false; if(interactive_request&&!value->allow_interactive) return false; return true; }
